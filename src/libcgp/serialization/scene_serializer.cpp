#include <libcgp/rc.hpp>
#include <libcgp/serialization/scene_serializer.hpp>
#include <libcgp/utils/files.hpp>
#include <libcgp/utils/macros.hpp>

#include <libcgp/mgr/object_mgr.hpp>
#include <libcgp/mgr/resource_mgr.hpp>
#include <libcgp/mgr/settings_mgr.hpp>

#include <filesystem>
#include <fstream>
#include <string>
#include <unordered_map>

LibGcp::Rc LibGcp::SceneSerializer::SerializeScene(const std::string &scene_name, const SerializationType type)
{
    if (!std::filesystem::exists(output_dir_) && !std::filesystem::create_directories(output_dir_)) {
        return Rc::kFailedToCreateDir;
    }

    if (!std::filesystem::is_directory(output_dir_)) {
        return Rc::kNotADirectory;
    }

    if (!FileWriteable(output_dir_)) {
        return Rc::kNoPermission;
    }

    if (std::filesystem::exists(output_dir_ + "/" + scene_name)) {
        return Rc::kFileAlreadyExists;
    }

    /* dispatch serialization */
    switch (type) {
        case SerializationType::kShallow:
            return _serializeSceneShallow(scene_name);
        case SerializationType::kDeep:
            return _serializeSceneDeep(scene_name);
        case SerializationType::kDeepPack:
            return _serializeSceneDeepPack(scene_name);
        case SerializationType::kDeepAttach:
            return _serializeSceneDeepAttach(scene_name);
        default:
            R_ASSERT(false);
    }
}

LibGcp::Scene LibGcp::SceneSerializer::LoadScene(const std::string &scene_name, SerializationType type){NOT_IMPLEMENTED}

LibGcp::Rc LibGcp::SceneSerializer::_serializeSceneShallow(const std::string &scene_name)
{
    SceneSerialized serial_struct{};

    serial_struct.header.source_version  = kGlobalVersion;
    serial_struct.header.scene_version   = kSceneVersion;
    serial_struct.header.texture_version = kTextureVersion;
    serial_struct.header.model_version   = kModelVersion;

    serial_struct.header.num_settings = static_cast<size_t>(Setting::kLast);

    string_map_.clear();
    string_counter_ = 0;

    /* gather data */
    const auto settings               = _serializeSettings();
    serial_struct.header.num_settings = settings.size();

    const auto resources               = _serializeResources();
    serial_struct.header.num_resources = resources.size();

    const auto static_objects        = _serializeStaticObjects();
    serial_struct.header.num_statics = static_objects.size();

    serial_struct.header.num_strings = string_map_.size();

    /* write to file */
    std::ofstream file(output_dir_ + "/" + scene_name, std::ios::binary);

    /* write header */
}

LibGcp::Rc LibGcp::SceneSerializer::_serializeSceneDeep(const std::string &scene_name){NOT_IMPLEMENTED}

LibGcp::Rc LibGcp::SceneSerializer::_serializeSceneDeepPack(const std::string &scene_name){NOT_IMPLEMENTED}

LibGcp::Rc LibGcp::SceneSerializer::_serializeSceneDeepAttach(const std::string &scene_name){NOT_IMPLEMENTED}

std::vector<LibGcp::SceneSerialized::SettingsSerialized> LibGcp::SceneSerializer::_serializeSettings()
{
    std::vector<SceneSerialized::SettingsSerialized> settings{};

    for (size_t i = 0; i < static_cast<size_t>(Setting::kLast); ++i) {
        settings.push_back({
            .setting = static_cast<Setting>(i),
            .value   = SettingsMgr::GetInstance().GetSetting<uint64_t>(static_cast<Setting>(i)),
        });
    }

    return settings;
}

std::vector<LibGcp::SceneSerialized::ResourceSerialized> LibGcp::SceneSerializer::_serializeResources()
{
    std::vector<SceneSerialized::ResourceSerialized> resources{};

    // ------------------------------
    // Models
    // ------------------------------

    ResourceMgr::GetInstance().GetModels().Lock();

    for (const auto &resource : ResourceMgr::GetInstance().GetModels()) {
        const size_t id = GetStringId_(resource.first);

        resources.push_back({
            .paths        = {id, 0},
            .type         = ResourceType::kModel,
            .load_type    = resource.second->load_type,
            .flip_texture = resource.second->flip_texture,
        });
    }

    ResourceMgr::GetInstance().GetModels().Unlock();

    // ------------------------------
    // Textures
    // ------------------------------

    ResourceMgr::GetInstance().GetTextures().Lock();

    for (const auto &resource : ResourceMgr::GetInstance().GetTextures()) {
        const size_t id = GetStringId_(resource.first);

        resources.push_back({
            .paths        = {id, 0},
            .type         = ResourceType::kTexture,
            .load_type    = resource.second->load_type,
            .flip_texture = resource.second->flip_texture,
        });
    }

    ResourceMgr::GetInstance().GetTextures().Unlock();

    // ------------------------------
    // Shaders
    // ------------------------------

    ResourceMgr::GetInstance().GetShaders().Lock();

    for (const auto &resource : ResourceMgr::GetInstance().GetShaders()) {
        /* split shader name */
        const auto shader_name     = resource.first;
        const auto pos             = shader_name.find("//");
        const auto vertex_shader   = shader_name.substr(0, pos);
        const auto fragment_shader = shader_name.substr(pos + 2);

        const size_t vertex_id   = GetStringId_(vertex_shader);
        const size_t fragment_id = GetStringId_(fragment_shader);

        resources.push_back({
            .paths        = {vertex_id, fragment_id},
            .type         = ResourceType::kShader,
            .load_type    = resource.second->load_type,
            .flip_texture = resource.second->flip_texture,
        });
    }

    ResourceMgr::GetInstance().GetShaders().Unlock();

    return resources;
}

std::vector<LibGcp::SceneSerialized::StaticObjectSerialized> LibGcp::SceneSerializer::_serializeStaticObjects()
{
    std::vector<SceneSerialized::StaticObjectSerialized> object_serialized{};

    ObjectMgr::GetInstance().GetStaticObjects().Lock();

    for (const auto &object : ObjectMgr::GetInstance().GetStaticObjects()) {
        const uint64_t model_id = object.GetModel()->resource_id;

        /* Find model name */
        std::string name;
        ResourceMgr::GetInstance().GetModels().Lock();

        for (const auto &model : ResourceMgr::GetInstance().GetModels()) {
            if (model.second->resource_id == model_id) {
                name = model.first;
                break;
            }
        }

        ResourceMgr::GetInstance().GetModels().Unlock();

        /* fill object */
        object_serialized.push_back({
            .name     = GetStringId_(name),
            .position = object.GetPosition(),
        });
    }

    ObjectMgr::GetInstance().GetStaticObjects().Unlock();

    return object_serialized;
}

size_t LibGcp::SceneSerializer::GetStringId_(const std::string &name)
{
    size_t id;

    if (!string_map_.contains(name)) {
        id                = string_counter_++;
        string_map_[name] = id;
    } else {
        id = string_map_[name];
    }

    return id;
}
