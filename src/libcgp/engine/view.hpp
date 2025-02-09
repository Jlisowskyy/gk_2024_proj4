#ifndef ENGINE_VIEW_HPP_
#define ENGINE_VIEW_HPP_

// clang-format off
#include <glad/gl.h>
#include <GLFW/glfw3.h> /* (include after glad) */
// clang-format on

#include <libcgp/defines.hpp>

#include <libcgp/intf.hpp>
#include <libcgp/mgr/object_mgr.hpp>
#include <libcgp/primitives/shader.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <array>

LIBGCP_DECL_START_

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

    void PrepareViewMatrices(Shader& shader);

    void PrepareModelMatrices(Shader& shader, const ObjectPosition& position);

    FAST_CALL void BindCameraWithObjet(const CameraInfo* camera_info) { camera_object_info_ = camera_info; }

    FAST_CALL const CameraInfo& GetBindObject() const noexcept { return *camera_object_info_; }

    void UpdateCameraPosition();

    void SetWindowAspectRatio(float aspect_ratio);

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
};

LIBGCP_DECL_END_

#endif  // ENGINE_VIEW_HPP_
