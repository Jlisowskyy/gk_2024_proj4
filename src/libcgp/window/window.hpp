#ifndef WINDOW_WINDOW_HPP_
#define WINDOW_WINDOW_HPP_

#include <CxxUtils/singleton.hpp>
#include <libcgp/defines.hpp>
#include <libcgp/window/mouse.hpp>

#include <cassert>
#include <tuple>

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

    NDSCRD FAST_CALL float GetAspectRatio() const
    {
        const auto [width, height] = GetWindowSize();
        return static_cast<float>(width) / static_cast<float>(height);
    }

    NDSCRD FAST_CALL std::tuple<int, int> GetWindowSize() const
    {
        std::tuple<int, int> size{};
        glfwGetWindowSize(window_, &std::get<0>(size), &std::get<1>(size));
        return size;
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
