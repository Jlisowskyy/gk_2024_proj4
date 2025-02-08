#ifndef INTF_HPP_
#define INTF_HPP_

#include <libcgp/defines.hpp>
#include <libcgp/version.hpp>

#include <CxxUtils/type_list.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// clang-format off
#include <glad/gl.h>
#include <GLFW/glfw3.h> /* (include after glad) */
// clang-format on

#include <array>
#include <atomic>
#include <cstdint>
#include <vector>

LIBGCP_DECL_START_
// ------------------------------
// Textures
// ------------------------------

struct TextureSpec {
    const unsigned char *texture_data;
    int width;
    int height;
    int channels;
};

// ------------------------------
// Objects
// ------------------------------

struct ObjectPosition {
    glm::vec3 position{};
    glm::vec3 rotation{};
    glm::vec3 scale{};
};

struct StaticObjectSpec {
    ObjectPosition position{};
    std::string name{};
};

struct DynamicObjectSpec {
    int xd;
};

using static_objects_t  = std::vector<StaticObjectSpec>;
using dynamic_objects_t = std::vector<DynamicObjectSpec>;

// ------------------------------
// Mesh
// ------------------------------

struct alignas(32) Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 tex_coords;
};

// ------------------------------
// Engine
// ------------------------------

struct CameraInfo {
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;

    double yaw   = -90.0;
    double pitch = 0.0;

    using buttons_t = std::array<int, GLFW_KEY_LAST>;

    void MoveFreeCamera(float distance, const buttons_t &buttons);

    void ConvertYawPitchToVector();

    void ConvertVectorToYawPitch();
};

// ------------------------------
// Resources
// ------------------------------

enum class ResourceType : std::uint8_t {
    kTexture,
    kShader,
    kModel,
    kLast,
};

enum class LoadType : std::uint8_t {
    kExternal,
    kMemory,
    kInternal,
    kExternalRaw,
    kLast,
};

struct ResourceSpec {
    std::array<std::string, 2> paths{};
    ResourceType type{};
    LoadType load_type{};
    int8_t flip_texture{-1};
};

using resource_t = std::vector<ResourceSpec>;

struct Resource {
    private:
    static std::atomic<uint64_t> id_counter_;

    public:
    const uint64_t resource_id = id_counter_.fetch_add(1);

    LoadType load_type  = LoadType::kLast;
    int8_t flip_texture = -1;

    void SaveSpec(const ResourceSpec &spec) noexcept
    {
        load_type    = spec.load_type;
        flip_texture = spec.flip_texture;
    }
};

// ------------------------------
// Settings
// ------------------------------

enum class Setting : std::uint16_t {
    kCameraType,
    kMouseSensitivity,
    kClockTicking,
    kFreeCameraSpeed,
    kBaseShader,
    kLast,
};

enum class CameraType : std::uint8_t {
    kStatic,
    kFollow,
    kFree,
    kFirstPerson,
    kThirdPerson,
    kLast,
};

struct SettingContainer {
    template <typename T>
    explicit SettingContainer(const T value) noexcept
    {
        SetSetting(value);
    }

    SettingContainer() = default;

    template <typename T>
    FAST_CALL uint64_t SetSetting(const T value) noexcept
    {
        static_assert(sizeof(T) <= sizeof(uint64_t), "Setting value is too large");
        static_assert(std::is_trivially_copyable<T>::value, "Setting type must be trivially copyable");

        *reinterpret_cast<T *>(&data_) = value;
        return data_;
    }

    template <typename T>
    NDSCRD FAST_CALL T GetSetting() const noexcept
    {
        static_assert(sizeof(T) <= sizeof(uint64_t), "Setting value is too large");
        static_assert(std::is_trivially_copyable<T>::value, "Setting type must be trivially copyable");

        return *reinterpret_cast<const T *>(&data_);
    }

    NDSCRD FAST_CALL uint64_t GetRaw() const noexcept { return data_; }

    protected:
    uint64_t data_{};
};

using setting_t = std::vector<std::tuple<Setting, SettingContainer> >;

template <size_t N>
using SettingTypes = CxxUtils::TypeList<N, CameraType, double, bool, double, uint64_t>;
static_assert(SettingTypes<0>::size == static_cast<size_t>(Setting::kLast), "Setting types list is incomplete");

static constexpr std::array kSettingsDescriptions{
    "Camera type", "Mouse sensitivity", "Is clock enabled", "Free camera speed", "ID of the base render shader",
};
static_assert(
    kSettingsDescriptions.size() == static_cast<size_t>(Setting::kLast), "Setting descriptions list is incomplete"
);

struct Scene {
    setting_t settings;
    resource_t resources;
    dynamic_objects_t dynamic_objects;
    static_objects_t static_objects;
};

static inline const Scene kEmptyScene{
    {},
    {ResourceSpec{
        .paths     = {"first_vertex_shader", "first_fragment_shader"},
        .type      = ResourceType::kShader,
        .load_type = LoadType::kMemory,
    }},
    {},
    {},
};

// ------------------------------
// Serialization
// ------------------------------

enum class SerializationType : std::uint8_t {
    kShallow,     // Serialize only the scene object preserve load types and paths
    kDeep,        // Serialize the scene object and all its resources
    kDeepPack,    // Serialize the scene object and all its resources and pack them into a single file
    kDeepAttach,  // Serialize the scene together with all its resources and attach them to compilable .cpp file
    kLast,
};

struct SceneSerialized {
    static constexpr uint64_t kMagic = 0xC4A1234BFEAE341;

    struct BaseHeader {
        Version source_version;
        SceneVersion scene_version;
        TextureVersion texture_version;
        ModelVersion model_version;
        size_t header_bytes;
        size_t payload_bytes;

        uint64_t magic = kMagic;
    };

    struct SceneHeader {
        BaseHeader base_header;

        size_t num_strings;
        size_t num_settings;
        size_t num_resources;
        size_t num_statics;
    };

    struct SettingsSerialized {
        Setting setting;
        uint64_t value;
    };

    struct ResourceSerialized {
        size_t paths[2];
        ResourceType type;
        LoadType load_type;
        int8_t flip_texture;
    };

    struct StaticObjectSerialized {
        size_t name;
        ObjectPosition position;
    };

    struct StringSerialized {
        size_t length;
        /* char data[0]; */
    };

    SceneHeader header;

    /* SettingsSerialized settings[]; */
    /* ResourceSerialized resources[]; */
    /* StaticObjectSerialized static_objects[]; */
    /* size_t string_table[]; */
    /* StringSerialized string_data[]; */
};

LIBGCP_DECL_END_

#endif  // INTF_HPP_
