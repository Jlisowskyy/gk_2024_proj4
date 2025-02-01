#ifndef WINDOW_OVERLAY_DEBUG_OVERLAY_HPP_
#define WINDOW_OVERLAY_DEBUG_OVERLAY_HPP_

#include <libcgp/defines.hpp>

struct GLFWwindow;

LIBGCP_DECL_START_
class DebugOverlay
{
    public:
    // ------------------------------
    // Object creation
    // ------------------------------

    DebugOverlay() = default;

    ~DebugOverlay();

    // ------------------------------
    // Class interaction
    // ------------------------------

    void DestroyOverlay();

    void EnableOverlay(GLFWwindow *window);

    void Draw();

    NDSCRD FAST_CALL bool IsEnabled() const { return window_ != nullptr; }

    void FAST_CALL SwitchOverlay(GLFWwindow *window)
    {
        if (IsEnabled()) {
            DestroyOverlay();
        } else {
            EnableOverlay(window);
        }
    }

    // ------------------------------
    // Implementation methods
    // ------------------------------

    protected:
    void DrawSettings_();
    void DrawObjects_();
    void ShowStatics_();
    void ShowDynamics_();
    void ShowSelectedObjects_();

    // ------------------------------
    // Class fields
    // ------------------------------

    std::vector<std::string> static_object_names_{};
    const StaticObject *static_object_{};
    int selected_static_object_idx_{-1};

    GLFWwindow *window_{};
};

LIBGCP_DECL_END_

#endif  // WINDOW_OVERLAY_DEBUG_OVERLAY_HPP_
