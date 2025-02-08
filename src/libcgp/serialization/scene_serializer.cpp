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
    // std::vec

    /* load settings */
    std::vector<SceneSerialized::SettingsSerialized> settings{};
    settings.resize(header.num_settings);
    file.read(
        reinterpret_cast<char *>(settings.data()), header.num_settings * sizeof(SceneSerialized::SettingsSerialized)
    );

    /* convert settings */
    scene.settings.reserve(header.num_settings);
    for (const auto &setting : settings) {
        scene.settings.emplace_back(static_cast<Setting>(setting.setting), setting.value);
    }

    /* load resources */
    std::vector<SceneSerialized::ResourceSerialized> resources{};
    resources.resize(header.num_resources);
    file.read(
        reinterpret_cast<char *>(resources.data()), header.num_resources * sizeof(SceneSerialized::ResourceSerialized)
    );

    /* load static objects */
    std::vector<SceneSerialized::StaticObjectSerialized> static_objects{};
    static_objects.resize(header.num_statics);
    file.read(
        reinterpret_cast<char *>(static_objects.data()),
        header.num_statics * sizeof(SceneSerialized::StaticObjectSerialized)
    );

    /* load string table */
    std::vector<size_t> string_table{};
    string_table.resize(header.num_strings);
    file.read(reinterpret_cast<char *>(string_table.data()), header.num_strings * sizeof(size_t));

    /* load strings */
    std::vector<char> string_data{};
    string_data.resize(header.base_header.payload_bytes);
    file.read(string_data.data(), header.base_header.payload_bytes);

    std::vector<std::string> strings{};
    strings.reserve(header.num_strings);

    /* convert strings */
    for (size_t idx = 0; idx < header.num_strings; ++idx) {
        const auto offset = string_table[idx];
        const auto length = *reinterpret_cast<size_t *>(&string_data[offset]);

        strings.emplace_back(&string_data[offset + sizeof(size_t)], length);
    }

    /* convert resources */
    scene.resources.reserve(header.num_resources);
    for (const auto &resource : resources) {
        scene.resources.push_back({
            {strings[resource.paths[0]], strings[resource.paths[1]]},
            static_cast<ResourceType>(resource.type),
            static_cast<LoadType>(resource.load_type),
            resource.flip_texture
        });
    }

    /* convert static objects */
    scene.static_objects.reserve(header.num_statics);
    for (const auto &object : static_objects) {
        scene.static_objects.push_back({object.position, strings[object.name]});
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
