#ifndef PRIMITIVES_STATIC_OBJECT_HPP_
#define PRIMITIVES_STATIC_OBJECT_HPP_

#include <libcgp/defines.hpp>
#include <libcgp/mgr/resource_mgr.hpp>
#include <libcgp/primitives/model.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <memory>

LIBGCP_DECL_START_
class StaticObject
{
    public:
    // ------------------------------
    // Object creation
    // ------------------------------

    StaticObject() = delete;

    ~StaticObject() = default;

    StaticObject(const glm::vec3 &position, const std::shared_ptr<Model> &model) : position_(position), model_(model)
    {
        TRACE("Created static object at: " << position.x << " " << position.y << " " << position.z);
    }

    // ------------------------------
    // Class interaction
    // ------------------------------

    FAST_CALL void Draw(Shader &shader) const { model_->Draw(shader); }

    // ------------------------------
    // Class fields
    // ------------------------------

    protected:
    glm::vec3 position_;
    std::shared_ptr<Model> model_;
};

LIBGCP_DECL_END_

#endif  // PRIMITIVES_STATIC_OBJECT_HPP_
