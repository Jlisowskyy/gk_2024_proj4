#ifndef INTF_HPP_
#define INTF_HPP_

#include <libcgp/defines.hpp>
#include <libcgp/version.hpp>

#include <CxxUtils/data_types/multi_vector.hpp>
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
    bool is_serializable{true};
};

using resource_t = std::vector<ResourceSpec>;

struct Resource {
    private:
    static std::atomic<uint64_t> id_counter_;

    public:
    const uint64_t resource_id = id_counter_.fetch_add(1);

    LoadType load_type   = LoadType::kLast;
    int8_t flip_texture  = -1;
    bool is_serializable = true;

    void SaveSpec(const ResourceSpec &spec) noexcept
    {
        load_type       = spec.load_type;
        flip_texture    = spec.flip_texture;
        is_serializable = spec.is_serializable;
    }
};

// ------------------------------
// Lights
// ------------------------------

static constexpr size_t kMaxLightPerObject   = 4;
static constexpr size_t kMaxLightObjects     = 64;
static constexpr size_t kMaxTypeLightObjects = 32;

struct LightInfo {
    glm::vec3 position;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float intensity{1.0};
};

struct PointLightInfo {
    float constant;
    float linear;
    float quadratic;
};

struct SpotLightInfo {
    glm::vec3 direction;
    float constant;
    float linear;
    float quadratic;
    float cut_off;
    float outer_cut_off;
};

struct GlobalLightSpec {
    LightInfo light_info;
    bool is_moving;
    uint64_t rise_time;
    uint64_t down_time;
    double angle;
};

struct PointLightSpec {
    std::string model_name;
    LightInfo light_info;
    PointLightInfo point_light;
};

struct SpotLightSpec {
    std::string model_name;
    LightInfo light_info;
    SpotLightInfo spot_light;
};

using point_lights_t = std::vector<PointLightSpec>;
using spot_lights_t  = std::vector<SpotLightSpec>;

// ------------------------------
// Settings
// ------------------------------

enum class Setting : std::uint16_t {
    kCameraType,
    kMouseSensitivity,
    kClockTicking,
    kFreeCameraSpeed,
    kBaseShader,
    kCurrentWordTime,
    kWordTimeCoef,
    kHighlightLightSources,
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
using SettingTypes = CxxUtils::TypeList<N, CameraType, double, bool, double, uint64_t, uint64_t, double, bool>;
static_assert(SettingTypes<0>::size == static_cast<size_t>(Setting::kLast), "Setting types list is incomplete");

static constexpr std::array kSettingsDescriptions{
    "Camera type",
    "Mouse sensitivity",
    "Is clock enabled",
    "Free camera speed",
    "ID of the base render shader",
    "Current word time",
    "Word time coefficient",
    "Highlight light sources",
};
static_assert(
    kSettingsDescriptions.size() == static_cast<size_t>(Setting::kLast), "Setting descriptions list is incomplete"
);

struct Scene {
    setting_t settings;
    resource_t resources;
    dynamic_objects_t dynamic_objects;
    static_objects_t static_objects;
    point_lights_t point_lights;
    spot_lights_t spot_lights;
};

static inline const Scene kEmptyScene{
    {   },
    {
     ResourceSpec{
            .paths     = {"first_vertex_shader", "first_fragment_shader"},
            .type      = ResourceType::kShader,
            .load_type = LoadType::kMemory,
        },ResourceSpec{
            .paths     = {"./models/sphere.glb", ""},
            .type      = ResourceType::kModel,
            .load_type = LoadType::kExternal,
        }, },
    { },
    {   },
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

struct PACK SceneSerialized {
    static constexpr uint64_t kMagic = 0xC4A1234BFEAE341;

    struct PACK BaseHeader {
        Version source_version;
        SceneVersion scene_version;
        TextureVersion texture_version;
        ModelVersion model_version;
        size_t header_bytes;
        size_t payload_bytes;

        uint64_t magic = kMagic;
    };

    struct PACK SceneHeader {
        BaseHeader base_header;

        size_t num_strings;
        size_t num_settings;
        size_t num_resources;
        size_t num_statics;
        size_t num_point_lights;
        size_t num_spot_lights;
    };

    struct PACK SettingsSerialized {
        Setting setting;
        uint64_t value;
    };

    struct PACK ResourceSerialized {
        size_t paths[2];
        ResourceType type;
        LoadType load_type;
        int8_t flip_texture;
    };

    struct PACK StaticObjectSerialized {
        size_t name;
        ObjectPosition position;
    };

    struct PACK StringTable {
        size_t idx;
    };

    struct PACK PointLightSerialized {
        size_t model;
        LightInfo light_info;
        PointLightInfo point_light;
    };

    struct PACK SpotLightSerialized {
        size_t model;
        LightInfo light_info;
        SpotLightInfo spot_light;
    };

    struct PACK StringSerialized {
        size_t length;
        /* char data[0]; */
    };

    SceneHeader header;

    /* SettingsSerialized settings[]; */
    /* ResourceSerialized resources[]; */
    /* StaticObjectSerialized static_objects[]; */
    /* PointLightSerialized point_lights[]; */
    /* SpotLightSerialized spot_lights[]; */
    /* size_t string_table[]; */
    /* StringSerialized string_data[]; */
};

LIBGCP_DECL_END_

#endif  // INTF_HPP_
