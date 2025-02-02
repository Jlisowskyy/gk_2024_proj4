#ifndef WINDOW_WINDOW_HPP_
#define WINDOW_WINDOW_HPP_

#include <CxxUtils/singleton.hpp>
#include <libcgp/defines.hpp>
#include <libcgp/window/mouse.hpp>
#include <libcgp/window/overlay/app_overlay.hpp>
#include <libcgp/window/overlay/debug_overlay.hpp>

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

    FAST_CALL void InitDebug() { debug_overlay_.Init(); }

    FAST_CALL void SwitchDebugOverlay()
    {
        /* show debug overlay */
        debug_overlay_.SwitchOverlay(window_);

        /* ensure correct mouse setting */
        SetMouseLockBasedOnOverlays();
    }

    template <class FuncT>
    FAST_CALL void RunLoop(FuncT &&process_progress)
    {
        // Game loop
        while (glfwWindowShouldClose(window_) == 0) {
            glfwPollEvents();

            /* call function processing progress of app*/
            process_progress();

            /* draw app overlay over content */
            app_overlay_.Draw();

            /* draw debug overlay over all content */
            debug_overlay_.Draw();

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

    NDSCRD FAST_CALL bool IsMouseLocked() const
    {
        return glfwGetInputMode(window_, GLFW_CURSOR) == GLFW_CURSOR_DISABLED;
    }

    FAST_CALL void SwitchMouseLock(const bool lock)
    {
        glfwSetInputMode(window_, GLFW_CURSOR, lock ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
        mouse_.SetEnabled(lock);

        if (lock) {
            double x_pos;
            double y_pos;

            glfwGetCursorPos(window_, &x_pos, &y_pos);

            mouse_.Reset(x_pos, y_pos);
        }
    }

    FAST_CALL void SetMouseLockBasedOnOverlays()
    {
        SwitchMouseLock(!debug_overlay_.IsEnabled() && !app_overlay_.IsOwningMouse());
    }

    NDSCRD FAST_CALL Mouse &GetMouse() noexcept { return mouse_; }

    // ----------------------------------
    // Class implementation methods
    // ----------------------------------

    protected:
    void SyncMousePositionWithWindow_();

    // ------------------------------
    // Window Callbacks
    // ------------------------------

    static void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode);

    static void FrameBufferSizeCallback(GLFWwindow *window, int width, int height);

    static void MouseCallback(GLFWwindow *window, double xpos, double ypos);

    // ------------------------------
    // Class fields
    // ------------------------------

    DebugOverlay debug_overlay_{};
    AppOverlay app_overlay_{};
    Mouse mouse_{};
    GLFWwindow *window_{};
};

LIBGCP_DECL_END_

#endif  // WINDOW_WINDOW_HPP_
