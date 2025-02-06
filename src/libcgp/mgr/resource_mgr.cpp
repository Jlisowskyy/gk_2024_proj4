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

LibGcp::ResourceMgrBase::~ResourceMgrBase() { TRACE("ResourceMgrBase::~ResourceMgrBase()"); }

LibGcp::ResourceMgrBase &LibGcp::ResourceMgrBase::Init(const resource_t &resources)
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

std::shared_ptr<LibGcp::Texture> LibGcp::ResourceMgrBase::GetTexture(const ResourceSpec &resource)
{
    assert(resource.type == ResourceType::kTexture);

    const std::lock_guard lock(textures_.GetMutex());
    const std::string &texture_name = resource.paths[0];

    if (textures_.contains(texture_name)) {
        TRACE(texture_name + " texture already loaded");
        return textures_.at(texture_name);
    }

    TRACE(texture_name + " texture not loaded");
    R_ASSERT(IsSuccess(LoadTextureUnlocked_(resource)));
    TRACE(texture_name + " texture loaded");

    assert(textures_.contains(texture_name));
    return textures_.at(texture_name);
}

std::shared_ptr<LibGcp::Shader> LibGcp::ResourceMgrBase::GetShader(const ResourceSpec &resource)
{
    assert(resource.type == ResourceType::kShader);

    const std::lock_guard lock(shaders_.GetMutex());
    const std::string shader_name = resource.paths[0] + "//" + resource.paths[1];

    if (shaders_.contains(shader_name)) {
        TRACE(shader_name + " shader already loaded");
        return shaders_.at(shader_name);
    }

    TRACE(shader_name + " shader not loaded");

    R_ASSERT(IsSuccess(LoadShaderUnlocked_(resource)));
    TRACE("shader loaded: " + shader_name);

    assert(shaders_.contains(shader_name));
    return shaders_.at(shader_name);
}

std::shared_ptr<LibGcp::Model> LibGcp::ResourceMgrBase::GetModel(const ResourceSpec &resource)
{
    assert(resource.type == ResourceType::kModel);

    const std::lock_guard lock(models_.GetMutex());
    const std::string &model_name = resource.paths[0];

    if (models_.contains(model_name)) {
        TRACE(model_name + " model already loaded");
        return models_.at(model_name);
    }

    TRACE(model_name + " model not loaded");
    R_ASSERT(IsSuccess(LoadModelUnlocked_(resource)));
    TRACE(model_name + " model loaded");

    assert(models_.contains(model_name));
    return models_.at(model_name);
}

std::shared_ptr<LibGcp::Texture> LibGcp::ResourceMgrBase::GetTexture(
    const std::string &texture_name, const LoadType load_type
)
{
    return GetTexture({.paths = {texture_name}, .type = ResourceType::kTexture, .load_type = load_type});
}

std::shared_ptr<LibGcp::Shader> LibGcp::ResourceMgrBase::GetShader(
    const std::string &shader_name, const LoadType load_type
)
{
    /* split for frag and vert shaders */
    const size_t end = shader_name.find("//");
    assert(end != std::string::npos);

    const std::string vert = shader_name.substr(0, end);
    const std::string frag = shader_name.substr(end + 2);

    return GetShader({
        .paths = {vert, frag},
          .type = ResourceType::kShader, .load_type = load_type
    });
}

std::shared_ptr<LibGcp::Model> LibGcp::ResourceMgrBase::GetModel(
    const std::string &model_name, const LoadType load_type
)
{
    return GetModel({.paths = {model_name}, .type = ResourceType::kModel, .load_type = load_type});
}

std::shared_ptr<LibGcp::Texture> LibGcp::ResourceMgrBase::GetTextureExternalSourceRaw(
    const std::string &path, const TextureSpec &spec
)
{
    const std::lock_guard lock(textures_.GetMutex());

    if (textures_.contains(path)) {
        TRACE(path + " texture already loaded");
        return textures_.at(path);
    }

    TRACE(path + " texture not loaded");

    std::shared_ptr<Texture> texture;
    if (spec.height == 0) {
        TRACE("Received compressed texture");

        texture = LoadTextureFromMemory_(spec.texture_data, spec.width);
    } else {
        TRACE("Received raw texture");
        texture =
            std::make_shared<Texture>(spec.texture_data, spec.width, spec.height, spec.channels, Texture::Type::kLast);
    }

    textures_[path] = texture;
    TRACE("Loaded texture: " + path);

    textures_.GetListeners().NotifyListeners<CxxUtils::ContainerEvents::kAdd>(path);
    return texture;
}

LibGcp::Rc LibGcp::ResourceMgrBase::LoadTextureUnlocked_(const ResourceSpec &resource)
{
    switch (resource.load_type) {
        case LoadType::kExternal:
            return LoadTextureFromExternal_(resource);
        case LoadType::kMemory:
            [[fallthrough]];
        case LoadType::kInternal:
            NOT_IMPLEMENTED;
        default:
            R_ASSERT(false);
    }
}

LibGcp::Rc LibGcp::ResourceMgrBase::LoadShaderUnlocked_(const ResourceSpec &resource)
{
    switch (resource.load_type) {
        case LoadType::kExternal:
            return LoadShaderFromExternal_(resource);
        case LoadType::kMemory:
            return LoadShaderFromMemory_(resource);
        case LoadType::kInternal:
            NOT_IMPLEMENTED;
        default:
            R_ASSERT(false);
    }
}

LibGcp::Rc LibGcp::ResourceMgrBase::LoadModelUnlocked_(const ResourceSpec &resource)
{
    switch (resource.load_type) {
        case LoadType::kExternal:
            return LoadModelFromExternal_(resource);
        case LoadType::kMemory:
            [[fallthrough]];
        case LoadType::kInternal:
            NOT_IMPLEMENTED;
        default:
            R_ASSERT(false);
    }
}

LibGcp::Rc LibGcp::ResourceMgrBase::LoadTextureFromExternal_(const ResourceSpec &resource)
{
    int width{};
    int height{};
    int channels{};

    const std::string &texture_name = resource.paths[0];

    if (resource.flip_texture != -1) {
        stbi_set_flip_vertically_on_load(resource.flip_texture);
    }

    unsigned char *data = stbi_load(texture_name.c_str(), &width, &height, &channels, 0);
    if (!data) {
        TRACE("Failed to load texture: " + texture_name);
        return Rc::kFailedToLoad;
    }

    const auto texture = std::make_shared<Texture>(data, width, height, channels, Texture::Type::kLast);
    stbi_image_free(data);

    assert(!textures_.contains(texture_name));
    textures_[texture_name] = texture;
    textures_.GetListeners().NotifyListeners<CxxUtils::ContainerEvents::kAdd>(texture_name);

    return Rc::kSuccess;
}

std::shared_ptr<LibGcp::Texture> LibGcp::ResourceMgrBase::LoadTextureFromMemory_(
    const unsigned char *data, const int len
)
{
    int width;
    int height;
    int channels;

    unsigned char *imageData = stbi_load_from_memory(data, len, &width, &height, &channels, 0);
    auto texture             = std::make_shared<Texture>(imageData, width, height, channels, Texture::Type::kLast);
    stbi_image_free(imageData);

    return texture;
}

LibGcp::Rc LibGcp::ResourceMgrBase::LoadShaderFromMemory_(const ResourceSpec &resource)
{
    const std::string &vert = resource.paths[0];
    const std::string &frag = resource.paths[1];

    R_ASSERT(StaticShaders::g_KnownFragmentShaders.contains(frag));
    R_ASSERT(StaticShaders::g_KnownVertexShaders.contains(vert));

    const auto shader = std::make_shared<Shader>(
        StaticShaders::g_KnownVertexShaders[vert], StaticShaders::g_KnownFragmentShaders[frag]
    );

    assert(!shaders_.contains(vert + "//" + frag));
    shaders_[vert + "//" + frag] = shader;
    shaders_.GetListeners().NotifyListeners<CxxUtils::ContainerEvents::kAdd>(vert + "//" + frag);

    return Rc::kSuccess;
}

LibGcp::Rc LibGcp::ResourceMgrBase::LoadShaderFromExternal_(const ResourceSpec &resource)
{
    const std::string &vert = resource.paths[0];
    const std::string &frag = resource.paths[1];

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
    shaders_.GetListeners().NotifyListeners<CxxUtils::ContainerEvents::kAdd>(vert + "//" + frag);

    return Rc::kSuccess;
}

LibGcp::Rc LibGcp::ResourceMgrBase::LoadModelFromExternal_(const ResourceSpec &resource)
{
    const std::string &model_name = resource.paths[0];
    ModelSerializer serializer{};

    if (resource.flip_texture != -1) {
        stbi_set_flip_vertically_on_load(resource.flip_texture);
    }

    const auto model = serializer.LoadModelFromExternalFormat(model_name);

    if (!model) {
        TRACE("Failed to load model: " + model_name);
        return Rc::kFailedToLoad;
    }

    assert(!models_.contains(model_name));
    models_[model_name] = model;
    models_.GetListeners().NotifyListeners<CxxUtils::ContainerEvents::kAdd>(model_name);

    return Rc::kSuccess;
}
