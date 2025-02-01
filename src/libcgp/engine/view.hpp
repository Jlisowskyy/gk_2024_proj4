#ifndef ENGINE_VIEW_HPP_
#define ENGINE_VIEW_HPP_

// clang-format off
#include <glad/gl.h>
#include <GLFW/glfw3.h> /* (include after glad) */
// clang-format on

#include <libcgp/defines.hpp>

#include <libcgp/mgr/object_mgr.hpp>
#include <libcgp/primitives/shader.hpp>
#include <libcgp/settings.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <array>

LIBGCP_DECL_START_

struct CameraInfo {
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;

    double yaw   = -90.0;
    double pitch = 0.0;

    using buttons_t = std::array<int, GLFW_KEY_LAST>;
    void MoveFreeCamera(float distance, const buttons_t& buttons);
    void ConvertYawPitchToVector();
    void ConvertVectorToYawPitch();
};

class View
{
    // ------------------------------
    // Class internals
    // ------------------------------

    // ------------------------------
    // Object creation
    // ------------------------------

    public:
    View();
    ~View() = default;

    // ------------------------------
    // Class interaction
    // ------------------------------

    void ChangeCameraType(CameraType type);

    void PrepareViewMatrices(Shader& shader);

    void PrepareModelMatrices(Shader& shader, const ObjectPosition& position);

    FAST_CALL void BindCameraWithObjet(const CameraInfo* camera_info) { camera_object_info_ = camera_info; }

    FAST_CALL const CameraInfo& GetBindObject() const noexcept { return *camera_object_info_; }

    NDSCRD FAST_CALL CameraType GetCameraType() const noexcept { return camera_type_; }

    void UpdateCameraPosition();

    // ---------------------------------
    // Class implementation methods
    // ---------------------------------

    protected:
    // ------------------------------
    // Class fields
    // ------------------------------

    const CameraInfo* camera_object_info_;

    glm::mat4 view_matrix_{};
    glm::mat4 projection_matrix_{};

    CameraType camera_type_{};
};

LIBGCP_DECL_END_

#endif  // ENGINE_VIEW_HPP_
