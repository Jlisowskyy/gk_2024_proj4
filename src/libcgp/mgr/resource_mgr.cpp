#include <libcgp/mgr/resource_mgr.hpp>

#include <cassert>
#include <fstream>
#include <memory>
#include <mutex>
#include <sstream>
#include <string>

#include <libcgp/primitives/model.hpp>
#include <libcgp/primitives/shader.hpp>
#include <libcgp/primitives/texture.hpp>
#include <libcgp/rc.hpp>
#include <libcgp/utils/macros.hpp>

#include <shaders/static_header.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

LibGcp::ResourceMgr::~ResourceMgr() { TRACE("ResourceMgr::~ResourceMgr()"); }

LibGcp::ResourceMgr &LibGcp::ResourceMgr::Init(const resource_t &resources)
{
    TRACE("Resource MGR init");

    textures_.reserve(kDefaultMapSize);
    shaders_.reserve(kDefaultMapSize);
    models_.reserve(kDefaultMapSize);

    for (const auto &resource : resources) {
        switch (resource.type) {
            case ResourceType::kTexture:
                R_ASSERT(IsSuccess(LoadTextureUnlocked_(resource)));
                break;
            case ResourceType::kShader:
                R_ASSERT(IsSuccess(LoadShaderUnlocked_(resource)));
                break;
            case ResourceType::kModel:
                R_ASSERT(IsSuccess(LoadModelUnlocked_(resource)));
                break;
            case ResourceType::kLast:
                R_ASSERT(false);
        }

        TRACE("Preloaded: " + resource.paths[0] + (resource.paths[1].empty() ? "" : "//" + resource.paths[1]));
    }

    return *this;
}

std::shared_ptr<LibGcp::Texture> LibGcp::ResourceMgr::GetTexture(
    const std::string &texture_name, const LoadType load_type
)
{
    const std::lock_guard lock(texture_mutex_);

    if (textures_.contains(texture_name)) {
        TRACE(texture_name + " texture already loaded");
        return textures_.at(texture_name);
    }

    TRACE(texture_name + " texture not loaded");
    R_ASSERT(IsSuccess(LoadTextureUnlocked_({
        .paths     = {texture_name},
        .load_type = load_type,
    })));
    TRACE(texture_name + " texture loaded");

    assert(textures_.contains(texture_name));
    return textures_.at(texture_name);
}

std::shared_ptr<LibGcp::Shader> LibGcp::ResourceMgr::GetShader(const std::string &shader_name, const LoadType load_type)
{
    const std::lock_guard lock(shader_mutex_);

    if (shaders_.contains(shader_name)) {
        TRACE(shader_name + " shader already loaded");
        return shaders_.at(shader_name);
    }

    TRACE(shader_name + " shader not loaded");

    /* split for frag and vert shaders */
    const size_t end = shader_name.find("//");
    assert(end != std::string::npos);

    R_ASSERT(IsSuccess(LoadShaderUnlocked_({
        .paths     = {shader_name.substr(0, end), shader_name.substr(end + 2)},
        .load_type = load_type,
    })));
    TRACE("shader loaded: " + shader_name);

    assert(shaders_.contains(shader_name));
    return shaders_.at(shader_name);
}

std::shared_ptr<LibGcp::Model> LibGcp::ResourceMgr::GetModel(const std::string &model_name, const LoadType load_type)
{
    const std::lock_guard lock(model_mutex_);

    if (models_.contains(model_name)) {
        TRACE(model_name + " model already loaded");
        return models_.at(model_name);
    }

    TRACE(model_name + " model not loaded");
    R_ASSERT(IsSuccess(LoadModelUnlocked_({
        .paths     = {model_name},
        .load_type = load_type,
    })));
    TRACE(model_name + " model loaded");

    assert(models_.contains(model_name));
    return models_.at(model_name);
}

std::shared_ptr<LibGcp::Texture> LibGcp::ResourceMgr::GetTextureExternalSource(
    const std::string &path, const TextureSpec &spec
)
{
    const std::lock_guard lock(texture_mutex_);

    if (textures_.contains(path)) {
        TRACE(path + " texture already loaded");
        return textures_.at(path);
    }

    TRACE(path + " texture not loaded");

    auto texture =
        std::make_shared<Texture>(spec.texture_data, spec.width, spec.height, spec.channels, Texture::Type::kLast);
    textures_[path] = texture;

    TRACE("Loaded texture: " + path);

    return texture;
}

LibGcp::Rc LibGcp::ResourceMgr::LoadTextureUnlocked_(const ResourceSpec &resource)
{
    switch (resource.load_type) {
        case LoadType::kExternal:
            return LoadTextureFromExternal_(resource.paths[0]);
        case LoadType::kMemory:
            [[fallthrough]];
        case LoadType::kInternal:
            NOT_IMPLEMENTED;
        default:
            R_ASSERT(false);
    }
}

LibGcp::Rc LibGcp::ResourceMgr::LoadShaderUnlocked_(const ResourceSpec &resource)
{
    switch (resource.load_type) {
        case LoadType::kExternal:
            return LoadShaderFromExternal_(resource.paths[0], resource.paths[1]);
        case LoadType::kMemory:
            return LoadShaderFromMemory_(resource.paths[0], resource.paths[1]);
        case LoadType::kInternal:
            NOT_IMPLEMENTED;
        default:
            R_ASSERT(false);
    }
}

LibGcp::Rc LibGcp::ResourceMgr::LoadModelUnlocked_(const ResourceSpec &resource)
{
    switch (resource.load_type) {
        case LoadType::kExternal:
            return LoadModelFromExternal_(resource.paths[0]);
        case LoadType::kMemory:
            [[fallthrough]];
        case LoadType::kInternal:
            NOT_IMPLEMENTED;
        default:
            R_ASSERT(false);
    }
}

LibGcp::Rc LibGcp::ResourceMgr::LoadTextureFromExternal_(const std::string &texture_name)
{
    int width{};
    int height{};
    int channels{};

    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(texture_name.c_str(), &width, &height, &channels, 0);
    if (!data) {
        TRACE("Failed to load texture: " + texture_name);
        return Rc::kFailedToLoad;
    }

    const auto texture = std::make_shared<Texture>(data, width, height, channels, Texture::Type::kLast);
    stbi_image_free(data);

    assert(!textures_.contains(texture_name));
    textures_[texture_name] = texture;

    return Rc::kSuccess;
}

LibGcp::Rc LibGcp::ResourceMgr::LoadShaderFromMemory_(const std::string &vert, const std::string &frag)
{
    R_ASSERT(StaticShaders::g_KnownFragmentShaders.contains(frag));
    R_ASSERT(StaticShaders::g_KnownVertexShaders.contains(vert));

    const auto shader = std::make_shared<Shader>(
        StaticShaders::g_KnownVertexShaders[vert], StaticShaders::g_KnownFragmentShaders[frag]
    );

    assert(!shaders_.contains(vert + "//" + frag));
    shaders_[vert + "//" + frag] = shader;

    return Rc::kSuccess;
}

LibGcp::Rc LibGcp::ResourceMgr::LoadShaderFromExternal_(const std::string &vert, const std::string &frag)
{
    // TODO: replace throw with Rc

    std::string vertex_shader_code;
    std::string fragment_shader_code;

    std::ifstream vertex_shader_file;
    std::ifstream fragment_shader_file;

    std::stringstream vertex_shader_stream;
    std::stringstream fragment_shader_stream;

    vertex_shader_file.open(vert);

    if (!vertex_shader_file.is_open()) {
        return Rc::kFailedToLoad;
    }

    fragment_shader_file.open(frag);

    if (!fragment_shader_file.is_open()) {
        return Rc::kFailedToLoad;
    }

    vertex_shader_stream << vertex_shader_file.rdbuf();

    if (vertex_shader_file.fail()) {
        return Rc::kFailedToLoad;
    }

    fragment_shader_stream << fragment_shader_file.rdbuf();

    if (fragment_shader_file.fail()) {
        return Rc::kFailedToLoad;
    }

    vertex_shader_file.close();
    fragment_shader_file.close();

    vertex_shader_code   = vertex_shader_stream.str();
    fragment_shader_code = fragment_shader_stream.str();

    const auto shader = std::make_shared<Shader>(vertex_shader_code.c_str(), fragment_shader_code.c_str());

    assert(!shaders_.contains(vert + "//" + frag));
    shaders_[vert + "//" + frag] = shader;

    return Rc::kSuccess;
}

LibGcp::Rc LibGcp::ResourceMgr::LoadModelFromExternal_(const std::string &model_name)
{
    ModelSerializer serializer{};

    const auto model = serializer.LoadModelFromExternalFormat(model_name);

    if (!model) {
        TRACE("Failed to load model: " + model_name);
        return Rc::kFailedToLoad;
    }

    assert(!models_.contains(model_name));
    models_[model_name] = model;

    return Rc::kSuccess;
}
