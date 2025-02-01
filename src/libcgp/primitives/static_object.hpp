#ifndef PRIMITIVES_STATIC_OBJECT_HPP_
#define PRIMITIVES_STATIC_OBJECT_HPP_

#include <libcgp/defines.hpp>
#include <libcgp/intf.hpp>
#include <libcgp/mgr/resource_mgr.hpp>
#include <libcgp/primitives/model.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <atomic>
#include <memory>

LIBGCP_DECL_START_
class StaticObject
{
    static std::atomic<uint64_t> id_counter_;

    public:
    // ------------------------------
    // Object creation
    // ------------------------------

    StaticObject() = delete;

    ~StaticObject() = default;

    StaticObject(const ObjectPosition &position, const std::shared_ptr<Model> &model)
        : id_(id_counter_.fetch_add(1)), position_(position), model_(model)
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

    FAST_CALL uint64_t GetId() const { return id_; }

    // ------------------------------
    // Class fields
    // ------------------------------

    protected:
    const uint64_t id_;
    ObjectPosition position_;
    std::shared_ptr<Model> model_;
};

LIBGCP_DECL_END_

#endif  // PRIMITIVES_STATIC_OBJECT_HPP_
