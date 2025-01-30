#ifndef WINDOW_WINDOW_HPP_
#define WINDOW_WINDOW_HPP_

#include <CxxUtils/singleton.hpp>
#include <libcgp/defines.hpp>

#include <cassert>

// clang-format off
#include <glad/gl.h>
#include <GLFW/glfw3.h> /* (include after glad) */
// clang-format on

LIBGCP_DECL_START_

class Window : public CxxUtils::Singleton<Window>
{
    protected:
    // ------------------------------
    // Class creation
    // ------------------------------

    Window() = default;

    public:
    ~Window();

    FAST_CALL static Window &InitInstance()
    {
        assert(!IsInited());

        Singleton::InitInstance(new Window());
        return GetInstance();
    }

    // ------------------------------
    // Class interaction
    // ------------------------------

    void Init();

    template <class FuncT>
    FAST_CALL void RunLoop(FuncT &&process_progress)
    {
        // Game loop
        while (glfwWindowShouldClose(window_) == 0) {
            // Check if any events have been activated (key pressed, mouse moved etc.) and call corresponding response
            // functions
            glfwPollEvents();

            // call the process_progress function
            process_progress();

            // Swap the screen buffers
            glfwSwapBuffers(window_);
        }
    }
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
