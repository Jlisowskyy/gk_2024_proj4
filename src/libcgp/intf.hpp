#ifndef INTF_HPP_
#define INTF_HPP_

#include <libcgp/defines.hpp>

#include <CxxUtils/type_list.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// clang-format off
#include <glad/gl.h>
#include <GLFW/glfw3.h> /* (include after glad) */
// clang-format on

#include <array>
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
    kLast,
};

struct alignas(128) ResourceSpec {
    std::array<std::string, 2> paths{};
    ResourceType type{};
    LoadType load_type{};
    int8_t flip_texture{-1};
};

using resource_t = std::vector<ResourceSpec>;

// ------------------------------
// Settings
// ------------------------------

enum class Setting : std::uint16_t {
    kCameraType,
    kMouseSensitivity,
    kClockTicking,
    kFreeCameraSpeed,
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

template <size_t N>
using SettingTypes = CxxUtils::TypeList<N, CameraType, double, bool, double>;
static_assert(SettingTypes<0>::size == static_cast<size_t>(Setting::kLast), "Setting types list is incomplete");

static constexpr std::array kSettingsDescriptions{
    "Camera type",
    "Mouse sensitivity",
    "Is clock enabled",
    "Free camera speed",
};
static_assert(
    kSettingsDescriptions.size() == static_cast<size_t>(Setting::kLast), "Setting descriptions list is incomplete"
);

LIBGCP_DECL_END_

#endif  // INTF_HPP_
