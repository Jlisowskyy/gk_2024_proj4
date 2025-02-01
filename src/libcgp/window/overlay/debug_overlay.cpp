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

void LibGcp::DebugOverlay::DestroyOverlay()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    static_object_names_.clear();
    static_object_ = nullptr;

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

    auto objects = ObjectMgr::GetInstance().GetStaticObjects();

    for (const auto &object : objects) {
        static_object_names_.push_back("Object " + std::to_string(object.GetId()));
    }
    selected_static_object_idx_ = -1;

    window_ = window;
}

void LibGcp::DebugOverlay::Draw()
{
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
    ShowSelectedObjects_();
}

void LibGcp::DebugOverlay::ShowStatics_()
{
    ImGui::Begin("Static objects:");

    if (ImGui::BeginListBox("##listbox")) {
        for (int i = 0; i < static_cast<int>(static_object_names_.size()); i++) {
            const bool is_selected = (selected_static_object_idx_ == i);
            if (ImGui::Selectable(static_object_names_[i].c_str(), is_selected)) {
                selected_static_object_idx_ = i;
                static_object_              = &ObjectMgr::GetInstance().GetStaticObjects()[i];
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
    }

    ImGui::End();
}

void LibGcp::DebugOverlay::ShowDynamics_() {}

void LibGcp::DebugOverlay::ShowSelectedObjects_()
{
    if (selected_static_object_idx_ == -1) {
        return;
    }

    auto shader = ResourceMgr::GetInstance().GetShader("contours//contours", ResourceMgr::LoadType::kMemory);
    if (selected_static_object_idx_ != -1) {
        static_object_->Draw(*shader);
    }
}
