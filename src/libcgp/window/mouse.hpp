#ifndef WINDOW_MOUSE_HPP_
#define WINDOW_MOUSE_HPP_

#include <libcgp/defines.hpp>
#include <libcgp/engine/view.hpp>

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

    FAST_CALL void BindCamera(CameraInfo* camera_info) noexcept { camera_info_ = camera_info; }

    // ---------------------------------
    // Class implementation methods
    // ---------------------------------

    protected:
    // ------------------------------
    // Class fields
    // ------------------------------

    double last_x_{};
    double last_y_{};

    CameraInfo* camera_info_{};
};

LIBGCP_DECL_END_

#endif  // WINDOW_MOUSE_HPP_
