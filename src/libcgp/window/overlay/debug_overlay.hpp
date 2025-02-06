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

    void Init();

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
    void DrawSettingsEditorWindow_();

    void DrawLiveObjectsInspectorWindow_();

    void DrawSpawnModelsWindow_();

    void DrawStaticObjectsSection_();

    void DrawDynamicObjectsSection_();

    void HighlightedSelectedMesh_();

    void DrawSelectedObjectPositionEditor_();

    void DrawSelectedModelSpawnSection_();

    void DrawDeleteObjectButton_();

    void SetSelectedObject_(int idx);

    void SetSelectedMesh_(int idx);

    void SetSelectedMode_(int idx);

    void FillObjectNames_();

    // ------------------------------
    // Class fields
    // ------------------------------

    /* Info to display */
    std::vector<std::string> static_object_names_{};
    std::vector<std::string> static_object_mesh_names_{};
    std::vector<std::string> model_names_{};
    StaticObject *static_object_{};
    std::shared_ptr<Model> static_object_model_{};
    std::shared_ptr<Mesh> static_object_mesh_{};

    /* Selection idxes */
    int selected_static_object_idx_{-1};
    int selected_mesh_idx_{-1};
    int selected_model_idx_{-1};

    /* Spawn model info */
    ObjectPosition spawn_model_pos_{
        .position = {},
        .rotation = {},
        .scale    = {1.0f, 1.0f, 1.0f},
    };

    /* Event data */
    int add_model_listener_{};
    int remove_model_listener_{};
    int add_object_listener_{};
    int remove_object_listener_{};

    /* Window info */
    GLFWwindow *window_{};
    std::shared_ptr<Shader> shader_{};
};

LIBGCP_DECL_END_

#endif  // WINDOW_OVERLAY_DEBUG_OVERLAY_HPP_
