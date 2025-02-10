#ifndef WINDOW_OVERLAY_DEBUG_OVERLAY_HPP_
#define WINDOW_OVERLAY_DEBUG_OVERLAY_HPP_

#include <libcgp/defines.hpp>
#include <libcgp/utils/files.hpp>

#include <ImGuiFileDialog.h>

struct GLFWwindow;

LIBGCP_DECL_START_
class DebugOverlay
{
    static constexpr int kPointLightRadioIdx = 0;
    static constexpr int kSpotLightRadioIdx  = 1;

    public:
    // ------------------------------
    // Object creation
    // ------------------------------

    DebugOverlay() = default;

    ~DebugOverlay();

    // ------------------------------
    // Class interaction
    // ------------------------------

    void Init(GLFWwindow *window);

    void Destroy();

    void DisableOverlay();

    void EnableOverlay(GLFWwindow *window);

    void Draw();

    NDSCRD FAST_CALL bool IsEnabled() const { return window_ != nullptr; }

    void FAST_CALL SwitchOverlay(GLFWwindow *window)
    {
        if (IsEnabled()) {
            DisableOverlay();
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

    void DrawSceneWindow_();

    void DrawInfoWindow_();

    void DrawFailure_();

    void DrawLightEditor_();

    void DrawEditPointLight_();

    void DrawEditSpotlight_();

    void DrawStaticObjectsSection_();

    void DrawDynamicObjectsSection_();

    void HighlightedSelectedMesh_();

    void DrawSelectedObjectPositionEditor_();

    void DrawSelectedModelSpawnSection_();

    void DrawStaticObjectButtons_();

    void SetSelectedObject_(int idx);

    void SetSelectedMesh_(int idx);

    void SetSelectedModel_(int idx);

    void SetSelectedPointLight_(int idx);

    void SetSelectedSpotlight_(int idx);

    void FillObjectNames_();

    void TriggerFailure_(const std::string &message);

    template <class FuncT>
    static void DisplayFileDialog_(
        const std::string &key, const std::string &title, const std::string &extensions, FuncT &&func
    )
    {
        if (ImGui::Button(title.c_str())) {
            IGFD::FileDialogConfig config;
            config.path = ".";

            ImGuiFileDialog::Instance()->OpenDialog(key, title, extensions.c_str(), config);
        }

        if (ImGuiFileDialog::Instance()->Display(key, ImGuiWindowFlags_NoCollapse, ImVec2(500, 600))) {
            if (ImGuiFileDialog::Instance()->IsOk()) {
                func(GetFullPath(ImGuiFileDialog::Instance()->GetFilePathName()));
            }
            ImGuiFileDialog::Instance()->Close();
        }
    }

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
    bool flip_textures_{};

    /* Event data */
    int add_model_listener_{};
    int remove_model_listener_{};
    int add_object_listener_{};
    int remove_object_listener_{};
    int clear_objects_listener_{};
    int clear_models_listener_{};

    /* failure */
    bool show_failure_{};
    std::string failure_message_{};

    /* add light */
    int selected_light_type_{};

    /* model info */
    std::shared_ptr<Model> selected_model_{};

    int selected_point_light_idx_{};
    std::vector<std::string> selected_model_point_lights_{};
    PointLight *selected_point_light_{};

    int selected_spotlight_idx_{};
    std::vector<std::string> selected_model_spotlights_{};
    SpotLight *selected_spotlight_{};

    /* Window info */
    GLFWwindow *window_{};
    std::shared_ptr<Shader> shader_{};
};

LIBGCP_DECL_END_

#endif  // WINDOW_OVERLAY_DEBUG_OVERLAY_HPP_
