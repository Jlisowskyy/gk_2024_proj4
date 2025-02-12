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

    StaticObject(const StaticObject &) = default;

    StaticObject &operator=(const StaticObject &) = default;

    StaticObject(StaticObject &&) = default;

    StaticObject &operator=(StaticObject &&) = default;

    StaticObject(const ObjectPosition &position, const std::shared_ptr<Model> &model)
        : id_(id_counter_.fetch_add(1)), position_(position), model_(model)
    {
        TRACE(
            "Created static object at: " << position.position.x << " " << position.position.y << " "
                                         << position.position.z
        );

        static constexpr float kMinRadius = 1.0F;
        static constexpr float kMaxRadius = 15.0F;
        static constexpr float kMinAngle  = 0.3F;
        static constexpr float kMaxAngle  = 2.0F * M_PI;
        static constexpr float kMinSpeed  = 0.05F;
        static constexpr float kMaxSpeed  = 0.2F;

        /* random move data */
        radius_ = kMinRadius + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (kMaxRadius - kMinRadius)));
        angle_  = kMinAngle + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (kMaxAngle - kMinAngle)));
        speed_  = kMinSpeed + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (kMaxSpeed - kMinSpeed)));
    }

    // ------------------------------
    // Class interaction
    // ------------------------------

    void ProcessDelta(uint64_t delta);

    WRAP_CALL void Draw(Shader &shader) const { model_->Draw(shader); }

    const ObjectPosition &GetPosition() const { return position_; }

    ObjectPosition &GetPosition() { return position_; }

    bool &IsRotating() { return is_rotating_; }
    const bool &IsRotating() const { return is_rotating_; }

    FAST_CALL uint64_t GetId() const { return id_; }

    NDSCRD FAST_CALL std::shared_ptr<Model> GetModel() const { return model_; }

    // ------------------------------
    // Class fields
    // ------------------------------

    protected:
    uint64_t id_;
    ObjectPosition position_;
    std::shared_ptr<Model> model_;

    float radius_{};
    float angle_{};
    bool is_rotating_{};
    float speed_{};
    float move_pos_{};
};

LIBGCP_DECL_END_

#endif  // PRIMITIVES_STATIC_OBJECT_HPP_
