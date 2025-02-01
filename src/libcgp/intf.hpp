#ifndef INTF_HPP_
#define INTF_HPP_

#include <libcgp/defines.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// clang-format off
#include <glad/gl.h>
#include <GLFW/glfw3.h> /* (include after glad) */
// clang-format on

LIBGCP_DECL_START_

struct TextureSpec {
    const unsigned char *texture_data;
    int width;
    int height;
    int channels;
};

struct ObjectPosition {
    glm::vec3 position{};
    glm::vec3 rotation{};
    glm::vec3 scale{};
};

struct StaticObjectSpec {
    ObjectPosition position{};
    std::string name{};
};

struct alignas(32) Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 tex_coords;
};

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

LIBGCP_DECL_END_

#endif  // INTF_HPP_
