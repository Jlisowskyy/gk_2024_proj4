#include <libcgp/engine/engine.hpp>
#include <libcgp/engine/word_time.hpp>
#include <libcgp/intf.hpp>
#include <libcgp/mgr/object_mgr.hpp>
#include <libcgp/mgr/resource_mgr.hpp>
#include <libcgp/mgr/settings_mgr.hpp>
#include <libcgp/rc.hpp>
#include <libcgp/serialization/scene_serializer.hpp>
#include <libcgp/window/overlay/debug_overlay.hpp>

#include <CxxUtils/type_list.hpp>

// clang-format off
#include <glad/gl.h>
#include <GLFW/glfw3.h> /* (include after glad) */
// clang-format on

#include <ImGuiFileDialog.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <cassert>
#include <libcgp/utils/files.hpp>
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
        DisableOverlay();
    }
}

void LibGcp::DebugOverlay::Init(GLFWwindow *window)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");

    shader_ = ResourceMgr::GetInstance().GetShader({
        .paths           = {"contours", "contours"},
        .type            = ResourceType::kShader,
        .load_type       = LoadType::kMemory,
        .is_serializable = false,
    });
}

void LibGcp::DebugOverlay::Destroy()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void LibGcp::DebugOverlay::DisableOverlay()
{
    ImGuiFileDialog::Instance()->Close();

    /* remove events */
    ResourceMgr::GetInstance().GetModels().GetListeners().RemoveListener<CxxUtils::ContainerEvents::kAdd>(
        add_model_listener_
    );
    ResourceMgr::GetInstance().GetModels().GetListeners().RemoveListener<CxxUtils::ContainerEvents::kRemove>(
        remove_model_listener_
    );
    ResourceMgr::GetInstance().GetModels().GetListeners().RemoveListener<CxxUtils::ContainerEvents::kClear>(
        clear_models_listener_
    );
    ObjectMgr::GetInstance().GetStaticObjects().GetListeners().RemoveListener<CxxUtils::ContainerEvents::kAdd>(
        add_object_listener_
    );
    ObjectMgr::GetInstance().GetStaticObjects().GetListeners().RemoveListener<CxxUtils::ContainerEvents::kRemove>(
        remove_object_listener_
    );
    ObjectMgr::GetInstance().GetStaticObjects().GetListeners().RemoveListener<CxxUtils::ContainerEvents::kClear>(
        clear_objects_listener_
    );

    window_ = nullptr;
}

void LibGcp::DebugOverlay::EnableOverlay(GLFWwindow *window)
{
    assert(window != nullptr);
    assert(window_ == nullptr);

    FillObjectNames_();
    FillLightNames_();
    model_names_ = ResourceMgr::GetInstance().GetModels().GetKeys();

    /* Events */
    add_model_listener_ =
        ResourceMgr::GetInstance().GetModels().GetListeners().AddListener<CxxUtils::ContainerEvents::kAdd>(
            [this](const std::string *name) {
                model_names_.push_back(*name);
            }
        );

    remove_model_listener_ =
        ResourceMgr::GetInstance().GetModels().GetListeners().AddListener<CxxUtils::ContainerEvents::kRemove>(
            [this](const std::string *name) {
                std::erase(model_names_, *name);
            }
        );

    clear_models_listener_ =
        ResourceMgr::GetInstance().GetModels().GetListeners().AddListener<CxxUtils::ContainerEvents::kClear>(
            [this](const std::string *) {
                model_names_.clear();
            }
        );

    add_object_listener_ =
        ObjectMgr::GetInstance().GetStaticObjects().GetListeners().AddListener<CxxUtils::ContainerEvents::kAdd>(
            [this](const StaticObject *object) {
                static_object_names_.push_back("Object " + std::to_string(object->GetId()));
            }
        );

    remove_object_listener_ =
        ObjectMgr::GetInstance().GetStaticObjects().GetListeners().AddListener<CxxUtils::ContainerEvents::kRemove>(
            [this](const StaticObject *object) {
                std::erase(static_object_names_, "Object " + std::to_string(object->GetId()));
            }
        );

    clear_objects_listener_ =
        ObjectMgr::GetInstance().GetStaticObjects().GetListeners().AddListener<CxxUtils::ContainerEvents::kClear>(
            [this](const StaticObject *) {
                static_object_names_.clear();
            }
        );

    window_ = window;
}

void LibGcp::DebugOverlay::Draw()
{
    HighlightedSelectedMesh_();
    DrawSelectedObjects_();
    DrawLightHighlights_();

    if (window_ == nullptr) {
        return;
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    DrawSettingsEditorWindow_();
    DrawLiveObjectsInspectorWindow_();
    DrawSpawnModelsWindow_();
    DrawSceneWindow_();
    DrawFailure_();
    DrawInfoWindow_();
    DrawGlobalLightEditorWindow_();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void LibGcp::DebugOverlay::DrawSelectedObjects_()
{
    if (selected_static_object_idx_ != -1) {
        DrawDebugPoint(static_object_->GetPosition().position, 0.1f, glm::vec3(0.5f, 0.2f, 1.0f));
    }
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
    ImGui::Begin("Spawn models:");
    ImGui::Checkbox("Flip textures", &flip_textures_);

    DisplayFileDialog_("ModelFileDlg", "Choose File", ".glb,.obj,.fbx,.blend", [&](const std::string &filePath) {
        ResourceMgr::GetInstance().GetModel({
            .paths        = {filePath},
            .type         = ResourceType::kModel,
            .load_type    = LoadType::kExternal,
            .flip_texture = static_cast<int8_t>(flip_textures_),
        });
    });

    if (ImGui::BeginListBox("Available models:")) {
        for (int i = 0; i < static_cast<int>(model_names_.size()); i++) {
            const bool is_selected = (selected_model_idx_ == i);
            if (ImGui::Selectable(model_names_[i].c_str(), is_selected)) {
                SetSelectedModel_(i);
            }
            if (is_selected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndListBox();
    }

    DrawSelectedModelSpawnSection_();

    ImGui::End();
}

void LibGcp::DebugOverlay::DrawSceneWindow_()
{
    ImGui::Begin("Scene editor: ");

    DisplayFileDialog_("SaveSceneDlg", "Save Scene", ".libgcp_scene", [&](const std::string &filePath) {
        SceneSerializer serializer(GetDirFromFile(filePath));
        const auto rc = serializer.SerializeScene(GetFileName(filePath), SerializationType::kShallow);

        if (IsFailure(rc)) {
            TRACE("Failed to save scene: " << GetRcDescription(rc));
            TriggerFailure_(GetRcDescription(rc));
        } else {
            TRACE("Scene saved successfully");
        }
    });

    DisplayFileDialog_("LoadSceneDlg", "Load scene", ".libgcp_scene", [&](const std::string &filePath) {
        SceneSerializer serializer(GetDirFromFile(filePath));
        const auto [rc, scene] = serializer.LoadScene(GetFileName(filePath), SerializationType::kShallow);

        if (IsFailure(rc)) {
            TRACE("Failed to load scene: " << GetRcDescription(rc));
            TriggerFailure_(GetRcDescription(rc));
        } else {
            TRACE("Scene loaded successfully");
            Engine::GetInstance().ReloadScene(scene);
        }
    });

    if (ImGui::Button("Clear all objects")) {
        ObjectMgr::GetInstance().GetStaticObjects().Clear();
    }

    if (ImGui::Button("Load empty scene")) {
        Engine::GetInstance().ReloadScene(kEmptyScene);
    }

    ImGui::End();
}

void LibGcp::DebugOverlay::DrawInfoWindow_()
{
    ImGui::Begin("Game state info:");

    const std::string time_str = "Current time: " + WordTime::GetTime();
    ImGui::Text(time_str.c_str());

    ImGui::SameLine();

    if (ImGui::SmallButton("+")) {
        const uint64_t curr_time = SettingsMgr::GetInstance().GetSetting<Setting::kCurrentWordTime, uint64_t>();
        SettingsMgr::GetInstance().SetSetting<Setting::kCurrentWordTime>(curr_time + WordTime::kSecondsInHour);
    }
    ImGui::SameLine();
    if (ImGui::SmallButton("-")) {
        const uint64_t curr_time = SettingsMgr::GetInstance().GetSetting<Setting::kCurrentWordTime, uint64_t>();
        SettingsMgr::GetInstance().SetSetting<Setting::kCurrentWordTime>(curr_time - WordTime::kSecondsInHour);
    }

    ImGui::End();
}

void LibGcp::DebugOverlay::SetSelectedGlobalLight_(const int idx)
{
    if (idx == global_light_idx_) {
        global_light_idx_ = -1;
        global_light_     = nullptr;
        return;
    }

    global_light_idx_ = idx;
    global_light_     = &Engine::GetInstance().GetGlobalLight().GetLight(idx);
}

void LibGcp::DebugOverlay::DrawGlobalLightEditorWindow_()
{
    ImGui::Begin("Global light editor");

    if (ImGui::Button("Add global light")) {
        Engine::GetInstance().GetGlobalLight().AddLight(kDefaultGlobalLight);
        global_light_names_.push_back(
            "Global light " + std::to_string(Engine::GetInstance().GetGlobalLight().GetLightsCount() - 1)
        );
    }

    if (ImGui::BeginListBox("Global lights")) {
        for (int i = 0; i < static_cast<int>(global_light_names_.size()); i++) {
            const bool is_selected = (global_light_idx_ == i);
            if (ImGui::Selectable(global_light_names_[i].c_str(), is_selected)) {
                SetSelectedGlobalLight_(i);
            }
            if (is_selected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndListBox();
    }

    ImGui::Separator();

    DrawGlobalLightEditSection_();

    ImGui::End();
}

void LibGcp::DebugOverlay::DrawGlobalLightEditSection_()
{
    if (global_light_idx_ == -1) {
        return;
    }

    if (ImGui::Button("Delete global light")) {
        Engine::GetInstance().GetGlobalLight().RemoveLight(global_light_idx_);
        global_light_idx_ = -1;
        global_light_     = nullptr;
        global_light_names_.pop_back();

        return;
    }

    ImGui::DragFloat3("Light position ", &global_light_->light_info.position.x, 0.01f);
    ImGui::DragFloat3("Ambient ", &global_light_->light_info.ambient.x, 0.01f);
    ImGui::DragFloat3("Diffuse ", &global_light_->light_info.diffuse.x, 0.01f);
    ImGui::DragFloat3("Specular ", &global_light_->light_info.specular.x, 0.01f);
    ImGui::DragFloat("Intensity ", &global_light_->light_info.intensity, 0.01f);

    ImGui::Checkbox("Is moving ", &global_light_->is_moving);

    int drag1 = global_light_->rise_time;
    ImGui::DragInt("Rise time ", &drag1, 1);
    global_light_->rise_time = drag1;

    int drag2 = global_light_->down_time;
    ImGui::DragInt("Down time ", &drag2, 1);
    global_light_->down_time = drag2;

    float drag3 = global_light_->angle;
    ImGui::DragFloat("Angle ", &drag3, 0.01f);
    global_light_->angle = drag3;
}

void LibGcp::DebugOverlay::DrawFailure_()
{
    if (show_failure_) {
        ImGui::OpenPopup("Failure");
    }

    if (ImGui::BeginPopupModal("Failure", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text(failure_message_.c_str());
        ImGui::Separator();

        if (ImGui::Button("OK")) {
            ImGui::CloseCurrentPopup();
            show_failure_ = false;
        }

        ImGui::EndPopup();
    }
}

void LibGcp::DebugOverlay::DrawLightEditor_()
{
    ImGui::Separator();

    if (ImGui::BeginListBox("Point lights")) {
        for (int i = 0; i < static_cast<int>(selected_model_point_lights_.size()); i++) {
            const bool is_selected = (selected_point_light_idx_ == i);
            if (ImGui::Selectable(selected_model_point_lights_[i].c_str(), is_selected)) {
                SetSelectedPointLight_(i);
            }
            if (is_selected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndListBox();
    }

    DrawEditPointLight_();

    ImGui::Separator();
    if (ImGui::BeginListBox("Spotlights")) {
        for (int i = 0; i < static_cast<int>(selected_model_spotlights_.size()); i++) {
            const bool is_selected = (selected_spotlight_idx_ == i);
            if (ImGui::Selectable(selected_model_spotlights_[i].c_str(), is_selected)) {
                SetSelectedSpotlight_(i);
            }
            if (is_selected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndListBox();
    }

    DrawEditSpotlight_();
}

void LibGcp::DebugOverlay::DrawEditPointLight_()
{
    if (selected_point_light_idx_ == -1) {
        return;
    }

    if (ImGui::Button("Delete point light")) {
        selected_model_->GetLights().GetUnderlyingData<PointLight>().erase(
            selected_model_->GetLights().GetUnderlyingData<PointLight>().begin() + selected_point_light_idx_
        );
        selected_model_point_lights_.pop_back();

        selected_point_light_idx_ = -1;
        selected_point_light_     = nullptr;
        return;
    }

    ImGui::DragFloat3("Light position", &selected_point_light_->light_info.position.x, 0.01f);
    ImGui::DragFloat3("Ambient", &selected_point_light_->light_info.ambient.x, 0.01f);
    ImGui::DragFloat3("Diffuse", &selected_point_light_->light_info.diffuse.x, 0.01f);
    ImGui::DragFloat3("Specular", &selected_point_light_->light_info.specular.x, 0.01f);
    ImGui::DragFloat("Intensity", &selected_point_light_->light_info.intensity, 0.01f);
    ImGui::DragFloat("Constant", &selected_point_light_->point_light.constant, 0.01f);
    ImGui::DragFloat("Linear", &selected_point_light_->point_light.linear, 0.01f);
    ImGui::DragFloat("Quadratic", &selected_point_light_->point_light.quadratic, 0.01f);
}

void LibGcp::DebugOverlay::DrawEditSpotlight_()
{
    if (selected_spotlight_idx_ == -1) {
        return;
    }

    if (ImGui::Button("Delete spotlight")) {
        selected_model_->GetLights().GetUnderlyingData<SpotLight>().erase(
            selected_model_->GetLights().GetUnderlyingData<SpotLight>().begin() + selected_spotlight_idx_
        );
        selected_model_spotlights_.pop_back();

        selected_spotlight_idx_ = -1;
        selected_spotlight_     = nullptr;
        return;
    }

    ImGui::DragFloat3("light position", &selected_spotlight_->light_info.position.x, 0.01f);
    ImGui::DragFloat3("ambient", &selected_spotlight_->light_info.ambient.x, 0.01f);
    ImGui::DragFloat3("diffuse", &selected_spotlight_->light_info.diffuse.x, 0.01f);
    ImGui::DragFloat3("specular", &selected_spotlight_->light_info.specular.x, 0.01f);
    ImGui::DragFloat("intensity", &selected_spotlight_->light_info.intensity, 0.01f);
    ImGui::DragFloat3("Direction", &selected_spotlight_->spot_light.direction.x, 0.01f);
    ImGui::DragFloat("constant", &selected_spotlight_->spot_light.constant, 0.01f);
    ImGui::DragFloat("linear", &selected_spotlight_->spot_light.linear, 0.01f);
    ImGui::DragFloat("quadratic", &selected_spotlight_->spot_light.quadratic, 0.01f);
    ImGui::DragFloat("Cut off", &selected_spotlight_->spot_light.cut_off, 0.01f);
    ImGui::DragFloat("Outer cut off", &selected_spotlight_->spot_light.outer_cut_off, 0.01f);
}

void LibGcp::DebugOverlay::SetSelectedModel_(const int idx)
{
    selected_model_point_lights_.clear();
    selected_model_spotlights_.clear();
    selected_point_light_idx_ = -1;
    selected_spotlight_idx_   = -1;

    if (idx == selected_model_idx_) {
        selected_model_idx_ = -1;
        selected_model_     = nullptr;
        return;
    }

    selected_model_idx_ = idx;
    selected_model_     = ResourceMgr::GetInstance().GetModel(model_names_[idx], LoadType::kExternal);

    for (size_t idx = 0; idx < selected_model_->GetLights().size<SpotLight>(); ++idx) {
        selected_model_spotlights_.push_back("Spotlight " + std::to_string(idx));
    }

    for (size_t idx = 0; idx < selected_model_->GetLights().size<PointLight>(); ++idx) {
        selected_model_point_lights_.push_back("Point light " + std::to_string(idx));
    }
}

void LibGcp::DebugOverlay::SetSelectedPointLight_(const int idx)
{
    if (selected_point_light_idx_ == idx) {
        selected_point_light_idx_ = -1;
        selected_point_light_     = nullptr;
        return;
    }

    assert(idx < static_cast<int>(selected_model_->GetLights().size<PointLight>()));
    selected_point_light_idx_ = idx;
    selected_point_light_     = &selected_model_->GetLights().GetUnderlyingData<PointLight>()[idx];
}

void LibGcp::DebugOverlay::SetSelectedSpotlight_(const int idx)
{
    if (selected_spotlight_idx_ == idx) {
        selected_spotlight_idx_ = -1;
        selected_spotlight_     = nullptr;
        return;
    }

    assert(idx < static_cast<int>(selected_model_->GetLights().size<SpotLight>()));
    selected_spotlight_idx_ = idx;
    selected_spotlight_     = &selected_model_->GetLights().GetUnderlyingData<SpotLight>()[idx];
}

void LibGcp::DebugOverlay::FillObjectNames_()
{
    std::lock_guard lock(ObjectMgr::GetInstance().GetStaticObjects().GetMutex());

    static_object_names_.clear();
    static_object_names_.reserve(ObjectMgr::GetInstance().GetStaticObjects().size());
    for (const auto &object : ObjectMgr::GetInstance().GetStaticObjects()) {
        static_object_names_.push_back("Object " + std::to_string(object.GetId()));
    }
}

void LibGcp::DebugOverlay::FillLightNames_()
{
    global_light_names_.clear();
    global_light_names_.reserve(Engine::GetInstance().GetGlobalLight().GetLightsCount());

    for (size_t i = 0; i < Engine::GetInstance().GetGlobalLight().GetLightsCount(); ++i) {
        global_light_names_.push_back("Global light " + std::to_string(i));
    }
}

void LibGcp::DebugOverlay::TriggerFailure_(const std::string &message)
{
    show_failure_    = true;
    failure_message_ = message;
}

void LibGcp::DebugOverlay::DrawDebugPoint(const glm::vec3 &position, const float size, const glm::vec3 color)
{
    const auto sphere_model = ResourceMgr::GetInstance().GetModel("./models/sphere.glb", LoadType::kExternal);

    ObjectPosition pos{};
    pos.position = position;
    pos.scale    = glm::vec3(size);
    pos.rotation = glm::vec3(0.0f);

    shader_->Activate();
    Engine::GetInstance().GetView().PrepareViewMatrices(*shader_);
    Engine::GetInstance().GetView().PrepareModelMatrices(*shader_, pos);
    shader_->SetVec3("color", color);

    glDisable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    sphere_model->Draw(*shader_);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_DEPTH_TEST);
}

void LibGcp::DebugOverlay::DrawLightHighlights_()
{
    if (!SettingsMgr::GetInstance().GetSetting<Setting::kHighlightLightSources, bool>()) {
        return;
    }

    for (auto &object : ObjectMgr::GetInstance().GetStaticObjects()) {
        const auto mm = View::PrepareModelMatrices(object.GetPosition());
        for (const auto &light : object.GetModel()->GetLights().GetUnderlyingData<PointLight>()) {
            const auto world_pos = mm * glm::vec4(light.light_info.position, 1.0f);
            DrawDebugPoint(world_pos, 0.1f);
        }

        for (const auto &light : object.GetModel()->GetLights().GetUnderlyingData<SpotLight>()) {
            const auto world_pos = mm * glm::vec4(light.light_info.position, 1.0f);
            DrawDebugPoint(world_pos, 0.1f);
        }
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

    DrawStaticObjectButtons_();
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
    /* pink */
    shader_->SetVec3("color", glm::vec3(1.0f, 0.0f, 1.0f));

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

    if (ImGui::Button("Center to camera")) {
        auto info = Engine::GetInstance().GetFreeCamera();

        spawn_model_pos_.position = info.position;
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

    ImGui::RadioButton("Point light", &selected_light_type_, kPointLightRadioIdx);
    ImGui::RadioButton("Spotlight", &selected_light_type_, kSpotLightRadioIdx);

    if (ImGui::Button("Add light")) {
        if (selected_light_type_ == kPointLightRadioIdx) {
            Engine::GetInstance().GetLightMgr().AddLight(*selected_model_, kDefaultPointLight);
            selected_model_point_lights_.push_back(
                "Point light " + std::to_string(selected_model_point_lights_.size())
            );
        } else {
            Engine::GetInstance().GetLightMgr().AddLight(*selected_model_, kDefaultSpotLight);
            selected_model_spotlights_.push_back("Spotlight " + std::to_string(selected_model_spotlights_.size()));
        }
    }

    DrawLightEditor_();
}

void LibGcp::DebugOverlay::DrawStaticObjectButtons_()
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

    if (ImGui::Button("Move to")) {
        Engine::GetInstance().MoveFreeCameraTo(static_object_->GetPosition().position);
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
