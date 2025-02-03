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
    const char *desc = LibGcp::SettingsMgr::kDescriptions[N];
    int value =
        static_cast<int>(LibGcp::SettingsMgr::GetInstance().GetSetting<T>(static_cast<LibGcp::SettingsMgr::Setting>(N))
        );

    if (ImGui::InputInt(desc, &value)) {
        LibGcp::SettingsMgr::GetInstance().SetSetting(
            static_cast<LibGcp::SettingsMgr::Setting>(N), static_cast<T>(value)
        );
    }
}

template <size_t N, class T>
    requires std::is_floating_point_v<T>
static void DisplaySetting()
{
    double value = static_cast<double>(
        LibGcp::SettingsMgr::GetInstance().GetSetting<T>(static_cast<LibGcp::SettingsMgr::Setting>(N))
    );
    const char *desc = LibGcp::SettingsMgr::kDescriptions[N];

    if (ImGui::InputDouble(desc, &value, 0.1, 1.0, "%.5f")) {
        LibGcp::SettingsMgr::GetInstance().SetSetting(
            static_cast<LibGcp::SettingsMgr::Setting>(N), static_cast<T>(value)
        );
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
    auto objects = ObjectMgr::GetInstance().GetStaticObjects();

    for (const auto &object : objects) {
        static_object_names_.push_back("Object " + std::to_string(object.GetId()));
    }
    selected_static_object_idx_ = -1;
    shader_ = ResourceMgr::GetInstance().GetShader("contours//contours", ResourceMgr::LoadType::kMemory);
}

void LibGcp::DebugOverlay::DestroyOverlay()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

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

    window_ = window;
}

void LibGcp::DebugOverlay::Draw()
{
    ShowSelectedObjects_();

    if (window_ == nullptr) {
        return;
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    DrawSettings_();
    DrawObjects_();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void LibGcp::DebugOverlay::DrawSettings_()
{
    ImGui::Begin("Settings Editor");

    CxxUtils::IterateTypeList<static_cast<size_t>(SettingsMgr::Setting::kLast) - 1, SettingsMgr::SettingTypes>::Apply(
        []<size_t N, class T>() {
            DisplaySetting<N, T>();
        }
    );

    ImGui::End();
}

void LibGcp::DebugOverlay::DrawObjects_()
{
    ShowStatics_();
    ShowDynamics_();
}

void LibGcp::DebugOverlay::ShowStatics_()
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

    if (selected_static_object_idx_ == -1) {
        ImGui::Text("No object selected...");
    } else {
        ImGui::Text(
            "Selected object data:\n"
            "Position: (%.2f, %.2f, %.2f)\n"
            "Rotation: (%.2f, %.2f, %.2f)\n",
            static_object_->GetPosition().position.x, static_object_->GetPosition().position.y,
            static_object_->GetPosition().position.z, static_object_->GetPosition().rotation.x,
            static_object_->GetPosition().rotation.y, static_object_->GetPosition().rotation.z
        );

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

void LibGcp::DebugOverlay::ShowDynamics_() {}

void LibGcp::DebugOverlay::ShowSelectedObjects_()
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
