#include <libcgp/mgr/object_mgr.hpp>
#include <libcgp/mgr/settings_mgr.hpp>
#include <libcgp/window/overlay/debug_overlay.hpp>

// clang-format off
#include <glad/gl.h>
#include <GLFW/glfw3.h> /* (include after glad) */
// clang-format on

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <cassert>

// ------------------------------------
// Static helpers implementations
// ------------------------------------

template <typename T>
static void DisplayFloatSetting(const LibGcp::SettingsMgr::Setting setting)
{
    double sensitivity = static_cast<double>(LibGcp::SettingsMgr::GetInstance().GetSetting<T>(setting));
    const char *desc   = LibGcp::SettingsMgr::kDescriptions[static_cast<size_t>(setting)];

    if (ImGui::InputDouble(desc, &sensitivity, 0.1, 1.0, "%.5f")) {
        LibGcp::SettingsMgr::GetInstance().SetSetting<T>(
            LibGcp::SettingsMgr::Setting::kMouseSensitivity, static_cast<T>(sensitivity)
        );
    }
}

template <typename T>
static void DisplayIntSetting(const LibGcp::SettingsMgr::Setting setting)
{
    int value        = static_cast<int>(LibGcp::SettingsMgr::GetInstance().GetSetting<T>(setting));
    const char *desc = LibGcp::SettingsMgr::kDescriptions[static_cast<size_t>(setting)];

    if (ImGui::InputInt(desc, &value)) {
        LibGcp::SettingsMgr::GetInstance().SetSetting<T>(setting, static_cast<T>(value));
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

    window_ = nullptr;
}

void LibGcp::DebugOverlay::EnableOverlay(GLFWwindow *window)
{
    assert(window != nullptr);
    assert(window_ == nullptr);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");

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

    DisplayIntSetting<CameraType>(SettingsMgr::Setting::kCameraType);
    DisplayFloatSetting<double>(SettingsMgr::Setting::kMouseSensitivity);
    DisplayIntSetting<bool>(SettingsMgr::Setting::kClockTicking);

    ImGui::End();
}

void LibGcp::DebugOverlay::DrawObjects_()
{
    ImGui::Begin("Debug overlay");

    const char *items[] = {"Item 1", "Item 2", "Item 3", "Item 4", "Item 5"};

    ImGui::Separator();
    ImGui::Text("Static objects:");

    static int listbox_item = 0;
    ImGui::ListBox("List", &listbox_item, items, IM_ARRAYSIZE(items));

    ImGui::End();
}
