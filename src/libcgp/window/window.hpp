#ifndef WINDOW_WINDOW_HPP_
#define WINDOW_WINDOW_HPP_

#include <libcgp/defines.hpp>

struct GLFWwindow;

LIBGCP_DECL_START_

class Window
{
    public:
    // ------------------------------
    // Class creation
    // ------------------------------

    Window() = default;

    ~Window();

    // ------------------------------
    // Class interaction
    // ------------------------------

    void Init();

    void RunLoop();

    // ----------------------------------
    // Class implementation methods
    // ----------------------------------

    protected:
    // ------------------------------
    // Class fields
    // ------------------------------

    GLFWwindow *window_{};
};

LIBGCP_DECL_END_

#endif  // WINDOW_WINDOW_HPP_
