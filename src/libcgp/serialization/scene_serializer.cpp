#include <libcgp/intf.hpp>
#include <libcgp/rc.hpp>
#include <libcgp/serialization/scene_serializer.hpp>
#include <libcgp/utils/files.hpp>
#include <libcgp/utils/macros.hpp>
#include <libcgp/version.hpp>

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
    //
    // if (std::filesystem::exists(output_dir_ + "/" + scene_name)) {
    //     return Rc::kFileAlreadyExists;
    // }

    /* dispatch serialization */
    switch (type) {
        case SerializationType::kShallow:
            return SerializeSceneShallow_(scene_name);
        case SerializationType::kDeep:
            return SerializeSceneDeep_(scene_name);
        case SerializationType::kDeepPack:
            return SerializeSceneDeepPack_(scene_name);
        case SerializationType::kDeepAttach:
            return SerializeSceneDeepAttach_(scene_name);
        default:
            R_ASSERT(false);
    }
}

std::tuple<LibGcp::Rc, LibGcp::Scene> LibGcp::SceneSerializer::LoadScene(
    const std::string &scene_name, const SerializationType type
)
{
    switch (type) {
        case SerializationType::kShallow:
            return LoadSceneShallow_(scene_name);
        case SerializationType::kDeep:
            return LoadSceneDeep_(scene_name);
        case SerializationType::kDeepPack:
            return LoadSceneDeepPack_(scene_name);
        case SerializationType::kDeepAttach:
            return LoadSceneDeepAttach_(scene_name);
        default:
            R_ASSERT(false);
    }
}

LibGcp::Rc LibGcp::SceneSerializer::SerializeSceneShallow_(const std::string &scene_name)
{
    SceneSerialized serial_struct{};

    serial_struct.header.base_header.source_version  = kGlobalVersion;
    serial_struct.header.base_header.scene_version   = kSceneVersion;
    serial_struct.header.base_header.texture_version = kTextureVersion;
    serial_struct.header.base_header.model_version   = kModelVersion;
    serial_struct.header.base_header.header_bytes    = sizeof(SceneSerialized::SceneHeader);

    serial_struct.header.num_settings = static_cast<size_t>(Setting::kLast);

    string_map_.clear();
    string_counter_    = 1;
    string_map_[""]    = 0;
    size_t total_bytes = 0;

    /* gather data */
    const auto settings               = SerializeSettings_();
    serial_struct.header.num_settings = settings.size();
    total_bytes += settings.size() * sizeof(SceneSerialized::SettingsSerialized);

    const auto resources               = SerializeResources_();
    serial_struct.header.num_resources = resources.size();
    total_bytes += resources.size() * sizeof(SceneSerialized::ResourceSerialized);

    const auto static_objects        = SerializeStaticObjects_();
    serial_struct.header.num_statics = static_objects.size();
    total_bytes += static_objects.size() * sizeof(SceneSerialized::StaticObjectSerialized);

    const auto lights                     = SerializeLights_();
    serial_struct.header.num_point_lights = lights.size<SceneSerialized::PointLightSerialized>();
    total_bytes += serial_struct.header.num_point_lights * sizeof(SceneSerialized::PointLightSerialized);
    serial_struct.header.num_spot_lights = lights.size<SceneSerialized::SpotLightSerialized>();
    total_bytes += serial_struct.header.num_spot_lights * sizeof(SceneSerialized::SpotLightSerialized);

    serial_struct.header.num_strings = string_map_.size();
    total_bytes += string_map_.size() * sizeof(uint64_t);
    total_bytes += string_map_.size() * sizeof(SceneSerialized::StringSerialized);

    for (const auto &string : string_map_) {
        total_bytes += string.first.size();
    }

    /* save payload size */
    serial_struct.header.base_header.payload_bytes = total_bytes;

    /* write to file */
    std::ofstream file(output_dir_ + "/" + scene_name, std::ios::binary);

    /* write header */
    file.write(reinterpret_cast<const char *>(&serial_struct.header), sizeof(SceneSerialized::SceneHeader));

    /* write settings */
    file.write(
        reinterpret_cast<const char *>(settings.data()), settings.size() * sizeof(SceneSerialized::SettingsSerialized)
    );

    /* write resources */
    file.write(
        reinterpret_cast<const char *>(resources.data()), resources.size() * sizeof(SceneSerialized::ResourceSerialized)
    );

    /* write static objects */
    file.write(
        reinterpret_cast<const char *>(static_objects.data()),
        static_objects.size() * sizeof(SceneSerialized::StaticObjectSerialized)
    );

    /* write point lights */
    file.write(
        reinterpret_cast<const char *>(lights.GetUnderlyingData<SceneSerialized::PointLightSerialized>().data()),
        lights.size<SceneSerialized::PointLightSerialized>() * sizeof(SceneSerialized::PointLightSerialized)
    );

    /* write spotlights */
    file.write(
        reinterpret_cast<const char *>(lights.GetUnderlyingData<SceneSerialized::SpotLightSerialized>().data()),
        lights.size<SceneSerialized::SpotLightSerialized>() * sizeof(SceneSerialized::SpotLightSerialized)
    );

    /* write string table */
    SaveStringTable(file);

    /* close file */
    file.close();

    return Rc::kSuccess;
}

LibGcp::Rc LibGcp::SceneSerializer::SerializeSceneDeep_(UNUSED const std::string &scene_name){NOT_IMPLEMENTED}

LibGcp::Rc LibGcp::SceneSerializer::SerializeSceneDeepPack_(UNUSED const std::string &scene_name){NOT_IMPLEMENTED}

LibGcp::Rc LibGcp::SceneSerializer::SerializeSceneDeepAttach_(UNUSED const std::string &scene_name){NOT_IMPLEMENTED}

std::vector<LibGcp::SceneSerialized::SettingsSerialized> LibGcp::SceneSerializer::SerializeSettings_()
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

std::vector<LibGcp::SceneSerialized::ResourceSerialized> LibGcp::SceneSerializer::SerializeResources_()
{
    std::vector<SceneSerialized::ResourceSerialized> resources{};

    // ------------------------------
    // Models
    // ------------------------------

    ResourceMgr::GetInstance().GetModels().Lock();

    for (const auto &[name, model] : ResourceMgr::GetInstance().GetModels()) {
        const size_t id = GetStringId_(name);

        resources.push_back({
            .paths        = {id, 0},
            .type         = ResourceType::kModel,
            .load_type    = model->load_type,
            .flip_texture = model->flip_texture,
        });
    }

    ResourceMgr::GetInstance().GetModels().Unlock();

    // ------------------------------
    // Textures
    // ------------------------------

    ResourceMgr::GetInstance().GetTextures().Lock();

    for (const auto &[name, texture] : ResourceMgr::GetInstance().GetTextures()) {
        if (texture->load_type == LoadType::kExternalRaw) {
            /* texture is embedded to the model */
            continue;
        }

        if (!texture->is_serializable) {
            /* texture is not serializable */
            continue;
        }

        const size_t id = GetStringId_(name);

        resources.push_back({
            .paths        = {id, 0},
            .type         = ResourceType::kTexture,
            .load_type    = texture->load_type,
            .flip_texture = texture->flip_texture,
        });
    }

    ResourceMgr::GetInstance().GetTextures().Unlock();

    // ------------------------------
    // Shaders
    // ------------------------------

    ResourceMgr::GetInstance().GetShaders().Lock();

    for (const auto &[name, shader] : ResourceMgr::GetInstance().GetShaders()) {
        if (!shader->is_serializable) {
            /* shader is not serializable */
            continue;
        }

        /* split shader name */
        const auto shader_name     = name;
        const auto pos             = shader_name.find("//");
        const auto vertex_shader   = shader_name.substr(0, pos);
        const auto fragment_shader = shader_name.substr(pos + 2);

        const size_t vertex_id   = GetStringId_(vertex_shader);
        const size_t fragment_id = GetStringId_(fragment_shader);

        resources.push_back({
            .paths        = {vertex_id, fragment_id},
            .type         = ResourceType::kShader,
            .load_type    = shader->load_type,
            .flip_texture = shader->flip_texture,
        });
    }

    ResourceMgr::GetInstance().GetShaders().Unlock();

    return resources;
}

std::vector<LibGcp::SceneSerialized::StaticObjectSerialized> LibGcp::SceneSerializer::SerializeStaticObjects_()
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

CxxUtils::MultiVector<LibGcp::SceneSerialized::PointLightSerialized, LibGcp::SceneSerialized::SpotLightSerialized>
LibGcp::SceneSerializer::SerializeLights_()
{
    CxxUtils::MultiVector<SceneSerialized::PointLightSerialized, SceneSerialized::SpotLightSerialized> vec{};

    ResourceMgr::GetInstance().GetModels().Lock();

    for (const auto &[name, model] : ResourceMgr::GetInstance().GetModels()) {
        const size_t id = GetStringId_(name);

        const auto func = [&]<class T>(const T &light) {
            vec.push_back(light.Serialize(id));
        };

        model->GetLights().Foreach(func);
    }

    ResourceMgr::GetInstance().GetModels().Unlock();

    return vec;
}

void LibGcp::SceneSerializer::SaveStringTable(std::ofstream &file)
{
    size_t offset{};

    /* get offsets */
    for (uint64_t idx = 0; idx < string_counter_; ++idx) {
        for (const auto &[string, id] : string_map_) {
            if (id == idx) {
                /* write and update offset */
                file.write(reinterpret_cast<const char *>(&offset), sizeof(size_t));

                offset += string.size() + sizeof(SceneSerialized::StringSerialized);
                break;
            }
        }
    }

    /* write strings */
    for (uint64_t idx = 0; idx < string_counter_; ++idx) {
        for (const auto &[string, id] : string_map_) {
            if (id == idx) {
                SceneSerialized::StringSerialized string_struct{
                    .length = string.size(),
                };

                file.write(reinterpret_cast<const char *>(&string_struct), sizeof(SceneSerialized::StringSerialized));
                file.write(string.c_str(), string.size());
                break;
            }
        }
    }
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

std::tuple<LibGcp::Rc, LibGcp::Scene> LibGcp::SceneSerializer::LoadSceneShallow_(const std::string &scene_name) const
{
    if (const auto rc = VerifyFile_(scene_name); IsFailure(rc)) {
        return {rc, {}};
    }

    /* open file as binary and read header */
    std::ifstream file(output_dir_ + "/" + scene_name, std::ios::binary);
    SceneSerialized::SceneHeader header{};
    file.read(reinterpret_cast<char *>(&header), sizeof(SceneSerialized::SceneHeader));

    /* check magic */
    if (header.base_header.magic != SceneSerialized::kMagic) {
        return {Rc::kCorruptedFile, kEmptyScene};
    }

    /* check version */
    if (header.base_header.scene_version < kMinSceneVersion) {
        return {Rc::kOutdatedProtocol, kEmptyScene};
    }

    Scene scene{};

    /* read whole scene -> we assume scene file are not that big */
    std::vector<char> payload{};
    payload.resize(header.base_header.payload_bytes);
    file.read(payload.data(), header.base_header.payload_bytes);
    file.close();

    /* prepare pointers */
    auto settings_table = reinterpret_cast<const SceneSerialized::SettingsSerialized *>(payload.data());
    auto resource_table =
        reinterpret_cast<const SceneSerialized::ResourceSerialized *>(&settings_table[header.num_settings]);
    auto static_objects =
        reinterpret_cast<const SceneSerialized::StaticObjectSerialized *>(&resource_table[header.num_resources]);
    auto point_lights =
        reinterpret_cast<const SceneSerialized::PointLightSerialized *>(&static_objects[header.num_statics]);
    auto spot_lights =
        reinterpret_cast<const SceneSerialized::SpotLightSerialized *>(&point_lights[header.num_point_lights]);
    auto string_table = reinterpret_cast<const SceneSerialized::StringTable *>(&spot_lights[header.num_spot_lights]);
    auto string_data  = reinterpret_cast<const char *>(&string_table[header.num_strings]);

    /* load settings */
    scene.settings.reserve(header.num_settings);
    for (size_t idx = 0; idx < header.num_settings; ++idx) {
        scene.settings.emplace_back(settings_table[idx].setting, settings_table[idx].value);
    }

    /* load strings */
    std::vector<std::string> strings{};
    strings.reserve(header.num_strings);

    for (size_t idx = 0; idx < header.num_strings; ++idx) {
        const auto string_struct =
            reinterpret_cast<const SceneSerialized::StringSerialized *>(&string_data[string_table[idx].idx]);
        const size_t length = string_struct->length;

        strings.emplace_back(&string_data[string_table[idx].idx + sizeof(SceneSerialized::StringSerialized)], length);
    }

    /* load resources */
    scene.resources.reserve(header.num_resources);
    for (size_t idx = 0; idx < header.num_resources; ++idx) {
        const auto &resource = resource_table[idx];

        scene.resources.push_back({
            {strings[resource.paths[0]], strings[resource.paths[1]]},
            resource.type,
            resource.load_type,
            resource.flip_texture,
        });
    }

    /* load static objects */
    scene.static_objects.reserve(header.num_statics);
    for (size_t idx = 0; idx < header.num_statics; ++idx) {
        const auto &object = static_objects[idx];

        scene.static_objects.push_back({
            object.position,
            strings[object.name],
        });
    }

    /* load point lights */
    scene.point_lights.reserve(header.num_point_lights);
    for (size_t idx = 0; idx < header.num_point_lights; ++idx) {
        const auto &light = point_lights[idx];

        scene.point_lights.push_back({
            strings[light.model],
            light.light_info,
            light.point_light,
        });
    }

    /* load spotlights */
    scene.spot_lights.reserve(header.num_spot_lights);
    for (size_t idx = 0; idx < header.num_spot_lights; ++idx) {
        const auto &light = spot_lights[idx];

        scene.spot_lights.push_back({
            strings[light.model],
            light.light_info,
            light.spot_light,
        });
    }

    return {Rc::kSuccess, scene};
}

std::tuple<LibGcp::Rc, LibGcp::Scene> LibGcp::SceneSerializer::LoadSceneDeep_(UNUSED const std::string &scene_name
){NOT_IMPLEMENTED}

std::tuple<LibGcp::Rc, LibGcp::Scene> LibGcp::SceneSerializer::LoadSceneDeepPack_(UNUSED const std::string &scene_name
){NOT_IMPLEMENTED}

std::tuple<LibGcp::Rc, LibGcp::Scene> LibGcp::SceneSerializer::LoadSceneDeepAttach_(UNUSED const std::string &scene_name
){NOT_IMPLEMENTED}

LibGcp::Rc LibGcp::SceneSerializer::VerifyFile_(const std::string &file_name) const
{
    if (!std::filesystem::exists(output_dir_)) {
        return Rc::kDirNotFound;
    }

    if (!std::filesystem::is_directory(output_dir_)) {
        return Rc::kNotADirectory;
    }

    if (!FileWriteable(output_dir_)) {
        return Rc::kNoPermission;
    }

    if (!std::filesystem::exists(output_dir_ + "/" + file_name)) {
        return Rc::kFileAlreadyExists;
    }

    if (!std::filesystem::is_regular_file(output_dir_ + "/" + file_name) ||
        !FileReadable(output_dir_ + "/" + file_name)) {
        return Rc::kFailedToOpenFile;
    }

    return Rc::kSuccess;
}
