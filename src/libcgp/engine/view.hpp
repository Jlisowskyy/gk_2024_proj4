#ifndef ENGINE_VIEW_HPP_
#define ENGINE_VIEW_HPP_

#include <libcgp/defines.hpp>

#include <libcgp/mgr/object_mgr.hpp>
#include <libcgp/primitives/shader.hpp>
#include <libcgp/settings.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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

    void ChangeCameraType(CameraType type);

    void PrepareViewMatrices(Shader& shader);

    void PrepareModelMatrices(Shader& shader, const ObjectPosition& position);

    FAST_CALL void BindCameraWithObjet(const glm::vec3* position, const glm::vec3* front)
    {
        camera_object_position_ = position;
        camera_object_front_    = front;
    }

    NDSCRD FAST_CALL CameraType GetCameraType() const noexcept { return camera_type_; }

    void UpdateCameraPosition();

    // ---------------------------------
    // Class implementation methods
    // ---------------------------------

    protected:
    void UpdateViewMatrix_();

    // ------------------------------
    // Class fields
    // ------------------------------

    const glm::vec3* camera_object_position_{};
    const glm::vec3* camera_object_front_{};

    glm::vec3 camera_position_{};
    glm::vec3 camera_front_{};

    glm::mat4 view_matrix_{};
    glm::mat4 projection_matrix_{};

    CameraType camera_type_{};
};

LIBGCP_DECL_END_

#endif  // ENGINE_VIEW_HPP_
