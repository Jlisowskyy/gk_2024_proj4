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
struct ObjectPosition {
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
};

struct StaticObjectSpec {
    ObjectPosition position;
    std::string name;
};

class StaticObject
{
    public:
    // ------------------------------
    // Object creation
    // ------------------------------

    StaticObject() = delete;

    ~StaticObject() = default;

    StaticObject(const ObjectPosition &position, const std::shared_ptr<Model> &model)
        : position_(position), model_(model)
    {
        TRACE(
            "Created static object at: " << position.position.x << " " << position.position.y << " "
                                         << position.position.z
        );
    }

    // ------------------------------
    // Class interaction
    // ------------------------------

    FAST_CALL void Draw(Shader &shader) const { model_->Draw(shader); }

    const ObjectPosition &GetPosition() const { return position_; }

    // ------------------------------
    // Class fields
    // ------------------------------

    protected:
    ObjectPosition position_;
    std::shared_ptr<Model> model_;
};

LIBGCP_DECL_END_

#endif  // PRIMITIVES_STATIC_OBJECT_HPP_
