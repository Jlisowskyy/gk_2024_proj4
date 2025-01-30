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

    // ---------------------------------
    // Class implementation methods
    // ---------------------------------

    protected:
    // ------------------------------
    // Class fields
    // ------------------------------

    glm::mat4 view_matrix_{};
    glm::mat4 projection_matrix_{};
};

LIBGCP_DECL_END_

#endif  // ENGINE_VIEW_HPP_
