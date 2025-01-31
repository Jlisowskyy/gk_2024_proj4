#ifndef WINDOW_MOUSE_HPP_
#define WINDOW_MOUSE_HPP_

#include <libcgp/defines.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

LIBGCP_DECL_START_
class Mouse
{
    // ------------------------------
    // Class internals
    // ------------------------------

    // ------------------------------
    // Object creation
    // ------------------------------

    public:
    Mouse() = default;

    Mouse(const double last_x, const double last_y) noexcept : last_x_(last_x), last_y_(last_y) {}

    // ------------------------------
    // Class interaction
    // ------------------------------

    void Move(double x_pos, double y_pos) noexcept;

    FAST_CALL void Reset(const double x_pos, const double y_pos) noexcept
    {
        last_x_ = x_pos;
        last_y_ = y_pos;
    }

    NDSCRD FAST_CALL const glm::vec3 &GetFront() const noexcept { return front_; }

    // ---------------------------------
    // Class implementation methods
    // ---------------------------------

    protected:
    // ------------------------------
    // Class fields
    // ------------------------------

    double yaw_   = -90.0;
    double pitch_ = 0.0;

    double last_x_{};
    double last_y_{};

    glm::vec3 front_{};
};

LIBGCP_DECL_END_

#endif  // WINDOW_MOUSE_HPP_
