#include <libcgp/engine/engine.hpp>
#include <libcgp/mgr/object_mgr.hpp>
#include <libcgp/mgr/resource_mgr.hpp>
#include <libcgp/mgr/settings_mgr.hpp>
#include <libcgp/window/overlay/debug_overlay.hpp>

#include <CxxUtils/type_list.hpp>

// clang-format off
#include <glad/gl.h>
#include <GLFW/glfw3.h> /* (include after glad) */
// clang-format on

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <cassert>
#include <type_traits>

// ------------------------------------
// Static helpers implementations
// ------------------------------------

template <size_t N, class T>
static void DisplaySetting()
{
    const char *desc = LibGcp::kSettingsDescriptions[N];
    int value = static_cast<int>(LibGcp::SettingsMgr::GetInstance().GetSetting<static_cast<LibGcp::Setting>(N), T>());

    if (ImGui::InputInt(desc, &value)) {
        LibGcp::SettingsMgr::GetInstance().SetSetting<static_cast<LibGcp::Setting>(N)>(static_cast<T>(value));
    }
}

template <size_t N, class T>
    requires std::is_floating_point_v<T>
static void DisplaySetting()
{
    double value =
        static_cast<double>(LibGcp::SettingsMgr::GetInstance().GetSetting<static_cast<LibGcp::Setting>(N), T>());
    const char *desc = LibGcp::kSettingsDescriptions[N];

    if (ImGui::InputDouble(desc, &value, 0.1, 1.0, "%.5f")) {
        LibGcp::SettingsMgr::GetInstance().SetSetting<static_cast<LibGcp::Setting>(N)>(static_cast<T>(value));
    }
}

// ------------------------------
// Method implementations
// ------------------------------

LibGcp::DebugOverlay::~DebugOverlay()
{
    if (window_ != nullptr) {
        DestroyOverlay();
    }
}

void LibGcp::DebugOverlay::Init()
{
    shader_ = ResourceMgr::GetInstance().GetShader("contours//contours", LoadType::kMemory);
}

void LibGcp::DebugOverlay::DestroyOverlay()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    /* remove events */
    ResourceMgr::GetInstance().GetModels().GetListeners().RemoveListener<CxxUtils::ContainerEvents::kAdd>(
        add_model_listener_
    );
    ResourceMgr::GetInstance().GetModels().GetListeners().RemoveListener<CxxUtils::ContainerEvents::kRemove>(
        remove_model_listener_
    );
    ObjectMgr::GetInstance().GetStaticObjects().GetListeners().RemoveListener<CxxUtils::ContainerEvents::kAdd>(
        add_object_listener_
    );
    ObjectMgr::GetInstance().GetStaticObjects().GetListeners().RemoveListener<CxxUtils::ContainerEvents::kRemove>(
        remove_object_listener_
    );

    window_ = nullptr;
}

void LibGcp::DebugOverlay::EnableOverlay(GLFWwindow *window)
{
    assert(window != nullptr);
    assert(window_ == nullptr);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");

    FillObjectNames_();
    model_names_ = ResourceMgr::GetInstance().GetModels().GetKeys();

    /* Events */
    add_model_listener_ =
        ResourceMgr::GetInstance().GetModels().GetListeners().AddListener<CxxUtils::ContainerEvents::kAdd>(
            [this](const std::string &name) {
                model_names_.push_back(name);
            }
        );

    remove_model_listener_ =
        ResourceMgr::GetInstance().GetModels().GetListeners().AddListener<CxxUtils::ContainerEvents::kRemove>(
            [this](const std::string &name) {
                std::erase(model_names_, name);
            }
        );

    add_object_listener_ =
        ObjectMgr::GetInstance().GetStaticObjects().GetListeners().AddListener<CxxUtils::ContainerEvents::kAdd>(
            [this](const StaticObject &object) {
                static_object_names_.push_back("Object " + std::to_string(object.GetId()));
            }
        );

    remove_object_listener_ =
        ObjectMgr::GetInstance().GetStaticObjects().GetListeners().AddListener<CxxUtils::ContainerEvents::kRemove>(
            [this](const StaticObject &object) {
                std::erase(static_object_names_, "Object " + std::to_string(object.GetId()));
            }
        );

    window_ = window;
}

void LibGcp::DebugOverlay::Draw()
{
    HighlightedSelectedMesh_();

    if (window_ == nullptr) {
        return;
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    DrawSettingsEditorWindow_();
    DrawLiveObjectsInspectorWindow_();
    DrawSpawnModelsWindow_();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void LibGcp::DebugOverlay::DrawSettingsEditorWindow_()
{
    ImGui::Begin("Settings Editor");

    CxxUtils::IterateTypeList<static_cast<size_t>(Setting::kLast) - 1, SettingTypes>::Apply([]<size_t N, class T>() {
        DisplaySetting<N, T>();
    });

    ImGui::End();
}

void LibGcp::DebugOverlay::DrawLiveObjectsInspectorWindow_()
{
    DrawStaticObjectsSection_();
    DrawDynamicObjectsSection_();
}

void LibGcp::DebugOverlay::DrawSpawnModelsWindow_()
{
    if (ImGui::BeginListBox("Available models:")) {
        for (int i = 0; i < static_cast<int>(model_names_.size()); i++) {
            const bool is_selected = (selected_model_idx_ == i);
            if (ImGui::Selectable(model_names_[i].c_str(), is_selected)) {
                SetSelectedMode_(i);
            }
            if (is_selected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndListBox();
    }

    DrawSelectedModelSpawnSection_();
}

void LibGcp::DebugOverlay::SetSelectedMode_(const int idx)
{
    if (idx == selected_model_idx_) {
        selected_model_idx_ = -1;
        return;
    }

    selected_model_idx_ = idx;
}

void LibGcp::DebugOverlay::FillObjectNames_()
{
    std::lock_guard lock(ObjectMgr::GetInstance().GetStaticObjects().GetMutex());

    static_object_names_.reserve(ObjectMgr::GetInstance().GetStaticObjects().size());
    for (const auto &object : ObjectMgr::GetInstance().GetStaticObjects()) {
        static_object_names_.push_back("Object " + std::to_string(object.GetId()));
    }
}

void LibGcp::DebugOverlay::DrawStaticObjectsSection_()
{
    ImGui::Begin("Static objects");

    if (ImGui::BeginListBox("Static objects:")) {
        for (int i = 0; i < static_cast<int>(static_object_names_.size()); i++) {
            const bool is_selected = (selected_static_object_idx_ == i);
            if (ImGui::Selectable(static_object_names_[i].c_str(), is_selected)) {
                SetSelectedObject_(i);
            }
            if (is_selected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndListBox();
    }

    ImGui::Separator();

    DrawDeleteObjectButton_();
    if (selected_static_object_idx_ == -1) {
        ImGui::Text("No object selected...");
    } else {
        DrawSelectedObjectPositionEditor_();

        if (ImGui::BeginListBox("Meshes")) {
            for (int i = 0; i < static_cast<int>(static_object_model_->GetMeshesCount()); i++) {
                const bool is_selected = (selected_mesh_idx_ == i);
                if (ImGui::Selectable(static_object_mesh_names_[i].c_str(), is_selected)) {
                    SetSelectedMesh_(i);
                }
                if (is_selected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndListBox();
        }
    }

    ImGui::End();
}

void LibGcp::DebugOverlay::DrawDynamicObjectsSection_() {}

void LibGcp::DebugOverlay::HighlightedSelectedMesh_()
{
    if (static_object_mesh_ == nullptr) {
        return;
    }

    shader_->Activate();
    Engine::GetInstance().GetView().PrepareViewMatrices(*shader_);
    Engine::GetInstance().GetView().PrepareModelMatrices(*shader_, static_object_->GetPosition());

    glDisable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    static_object_mesh_->Draw(*shader_);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_DEPTH_TEST);
}

void LibGcp::DebugOverlay::DrawSelectedObjectPositionEditor_()
{
    assert(static_object_ != nullptr);

    ImGui::Text("Selected object data:");

    ImGui::DragFloat3("Position", &static_object_->GetPosition().position.x, 0.01f);
    ImGui::DragFloat3("Rotation", &static_object_->GetPosition().rotation.x, 0.01f);
    ImGui::DragFloat3("Scale", &static_object_->GetPosition().scale.x, 0.01f);

    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Drag to adjust or double-click to type value. Hold Shift for faster changes.");
    }
}

void LibGcp::DebugOverlay::DrawSelectedModelSpawnSection_()
{
    if (selected_model_idx_ == -1) {
        return;
    }

    ImGui::DragFloat3("Position", &spawn_model_pos_.position.x, 0.01f);

    ImGui::DragFloat3("Rotation", &spawn_model_pos_.rotation.x, 0.01f);

    ImGui::DragFloat3("Scale", &spawn_model_pos_.scale.x, 0.01f);

    if (ImGui::Button("Spawn")) {
        ObjectMgr::GetInstance().AddStaticObject({
            .position = spawn_model_pos_,
            .name     = model_names_[selected_model_idx_],
        });
    }
}

void LibGcp::DebugOverlay::DrawDeleteObjectButton_()
{
    if (selected_static_object_idx_ == -1) {
        return;
    }

    if (ImGui::Button("Delete")) {
        const uint64_t ident = static_object_->GetId();
        SetSelectedObject_(selected_static_object_idx_);
        ObjectMgr::GetInstance().RemoveStaticObject(ident);

        return;
    }
}

void LibGcp::DebugOverlay::SetSelectedObject_(const int idx)
{
    static_object_mesh_names_.clear();
    static_object_mesh_ = nullptr;
    selected_mesh_idx_  = -1;

    if (idx == selected_static_object_idx_) {
        /* remove selection */

        selected_static_object_idx_ = -1;
        static_object_              = nullptr;
        static_object_model_        = nullptr;
        return;
    }

    selected_static_object_idx_ = idx;
    static_object_              = &ObjectMgr::GetInstance().GetStaticObjects()[idx];
    static_object_model_        = static_object_->GetModel();

    static_object_mesh_names_.reserve(static_object_model_->GetMeshesCount());
    for (size_t i = 0; i < static_object_model_->GetMeshesCount(); ++i) {
        static_object_mesh_names_.push_back("Mesh " + std::to_string(i));
    }
}

void LibGcp::DebugOverlay::SetSelectedMesh_(const int idx)
{
    if (idx == selected_mesh_idx_) {
        /* remove selection */
        selected_mesh_idx_  = -1;
        static_object_mesh_ = nullptr;

        return;
    }

    selected_mesh_idx_  = idx;
    static_object_mesh_ = static_object_model_->GetMesh(idx);
}
