#include <libcgp/engine/engine.hpp>

#include <libcgp/mgr/settings_mgr.hpp>
#include <libcgp/utils/macros.hpp>
#include <libcgp/window/window.hpp>

LibGcp::EngineBase::~EngineBase() { TRACE("EngineBase::~EngineBase()"); }

void LibGcp::EngineBase::Init(const Scene &scene) noexcept
{
    TRACE("EngineBase::Init()");

    /* connect with settings */
    SettingsMgr::GetInstance().AddListener(Setting::kCameraType, OnCameraTypeChanged_);
    SettingsMgr::GetInstance().AddListener(Setting::kCurrentWordTime, OnWordTimeChanged_);

    /* Adjust camera type based on default settings - intentional cast */
    OnCameraTypeChanged_(SettingsMgr::GetInstance().GetSetting<uint64_t>(Setting::kCameraType));

    /* Add reaction on shader change */
    SettingsMgr::GetInstance().AddListener(Setting::kBaseShader, OnDefaultShaderChanged_);

    /* init flowing TEMP object */
    flowing_camera_.position = glm::vec3{};
    flowing_camera_.front    = glm::vec3(0.0f, 0.0f, -1.0f);
    flowing_camera_.up       = glm::vec3(0.0f, 1.0f, 0.0f);

    /* load initial scene */
    ReloadScene(scene);

    /* load default shader */
    const uint64_t default_id = SettingsMgr::GetInstance().GetSetting<uint64_t>(Setting::kBaseShader);
    R_ASSERT(default_id < Shader::GetInstanceCount() && "Default shader not found");

    default_shader_ = FindShaderWithId(default_id);
    R_ASSERT(Engine::GetInstance().default_shader_ && "Default shader not found");

    /* TODO: load global lights from scene */
    global_light_.LoadLights({
        // sun
        GlobalLightSpec{
                        .light_info =
                        {
                        .ambient  = {0.2F, 0.2F, 0.2F},
                        .diffuse  = {0.5F, 0.5F, 0.5F},
                        .specular = {1.0F, 1.0F, 1.0F},
                        }, .is_moving = true,
                        .rise_time = WordTime::ConvertToSeconds(6, 0, 0),
                        .down_time = WordTime::ConvertToSeconds(22, 30, 0),
                        .angle     = 0.25,
                        },
        // Moon
        GlobalLightSpec{
                        .light_info =
                        {
                        .ambient  = {0.1F, 0.1F, 0.1F},
                        .diffuse  = {0.3F, 0.3F, 0.3F},
                        .specular = {0.5F, 0.5F, 0.5F},
                        }, .is_moving = true,
                        .rise_time = WordTime::ConvertToSeconds(21, 0, 0),
                        .down_time = WordTime::ConvertToSeconds(7,  0, 0),
                        .angle     = 0.6,
                        }
    });
}

void LibGcp::EngineBase::Draw()
{
    const auto shader = Engine::GetInstance().GetDefaultShader();
    shader->Activate();

    light_mgr_.PrepareLights(*shader);
    global_light_.PrepareLights(*shader);
    ObjectMgr::GetInstance().DrawStaticObjects(*shader);
}

void LibGcp::EngineBase::ProcessProgress(const uint64_t delta)
{
    ProcessInput_(delta);

    if (SettingsMgr::GetInstance().GetSetting<Setting::kClockTicking, bool>()) {
        word_time_.UpdateTime(delta);
        ProcessDynamicObjects_(delta);
    }

    if (SettingsMgr::GetInstance().GetSetting<Setting::kCameraType, CameraType>() == CameraType::kFree) {
        ProcessFreeCameraMovement_(delta);
    }

    view_.UpdateCameraPosition();

    /* Reset keys input for next frame */
    keys_.fill(0);
}

void LibGcp::EngineBase::ReloadScene(const Scene &scene)
{
    /* remove all objects */
    ObjectMgr::GetInstance().GetStaticObjects().Clear();

    /* clean all resources */
    ResourceMgr::GetInstance().GetModels().Clear();
    ResourceMgr::GetInstance().GetShaders().Clear();
    ResourceMgr::GetInstance().GetTextures().Clear();

    /* load resources */
    ResourceMgr::GetInstance().LoadResourceFromScene(scene);

    /* load objects */
    ObjectMgr::GetInstance().LoadObjectsFromScene(scene);

    /* load lights */
    light_mgr_.LoadLightsFromScene(scene);

    /* Note that settings must be loaded after all objects as there may be some events to fire */
    /* ensure default are loaded */
    SettingsMgr::GetInstance().LoadDefaultSettings();

    /* load settings */
    for (const auto &[setting, obj] : scene.settings) {
        SettingsMgr::GetInstance().SetSetting<uint64_t>(setting, obj.GetRaw());
    }
}

void LibGcp::EngineBase::ProcessInput_(const uint64_t delta)
{
    /* first process camera change */
    if (keys_[GLFW_KEY_F1]) {
        SettingsMgr::GetInstance().SetSetting<Setting::kCameraType>(CameraType::kStatic);
    } else if (keys_[GLFW_KEY_F2]) {
        SettingsMgr::GetInstance().SetSetting<Setting::kCameraType>(CameraType::kFollow);
    } else if (keys_[GLFW_KEY_F3]) {
        SettingsMgr::GetInstance().SetSetting<Setting::kCameraType>(CameraType::kFree);
    } else if (keys_[GLFW_KEY_F4]) {
        SettingsMgr::GetInstance().SetSetting<Setting::kCameraType>(CameraType::kFirstPerson);
    } else if (keys_[GLFW_KEY_F5]) {
        SettingsMgr::GetInstance().SetSetting<Setting::kCameraType>(CameraType::kThirdPerson);
    }

    /* process overlay changes */
    if (keys_[GLFW_KEY_F12]) {
        Window::GetInstance().SwitchDebugOverlay();
    }

    /* process movement */
    const long is_clock_enabled = SettingsMgr::GetInstance().GetSetting<Setting::kClockTicking, bool>();
    ProcessUserMovement_(is_clock_enabled * delta);
}

void LibGcp::EngineBase::ProcessUserMovement_(UNUSED const uint64_t delta) {}

void LibGcp::EngineBase::ProcessFreeCameraMovement_(const uint64_t delta)
{
    const double free_cam_speed = SettingsMgr::GetInstance().GetSetting<Setting::kFreeCameraSpeed, double>();
    const double distance       = free_cam_speed * static_cast<double>(delta) / 1e+6;

    free_camera_.MoveFreeCamera(static_cast<float>(distance), keys_);
}

void LibGcp::EngineBase::ProcessDynamicObjects_(const uint64_t delta)
{
    /* TODO: temporary, bind object movement to camera movement later */
    const double radius = 30.0f;
    flow_count_ += static_cast<double>(delta) / 1e+6;
    const float camX = sin(flow_count_) * radius;
    const float camZ = cos(flow_count_) * radius;

    flowing_camera_.position = glm::vec3(camX, 0.0f, camZ);
    flowing_camera_.front    = -flowing_camera_.position;
}

void LibGcp::EngineBase::OnCameraTypeChanged_(const uint64_t new_value)
{
    TRACE("Camera type changed to " << new_value);

    /* TODO: Bind proper object here */
    switch (static_cast<CameraType>(new_value)) {
        case CameraType::kStatic: {
            Window::GetInstance().GetMouse().BindCamera(nullptr);
            Engine::GetInstance().view_.BindCameraWithObjet(&Engine::GetInstance().static_camera_);
        } break;
        case CameraType::kFollow: {
            Window::GetInstance().GetMouse().BindCamera(nullptr);
            Engine::GetInstance().view_.BindCameraWithObjet(&Engine::GetInstance().flowing_camera_);
        } break;
        case CameraType::kFree: {
            /* preserve old camera */
            Engine::GetInstance().free_camera_ = Engine::GetInstance().view_.GetBindObject();

            /* ensure yaw pitch is set and ready */
            Engine::GetInstance().free_camera_.ConvertVectorToYawPitch();

            /* bind camera to mouse */
            Window::GetInstance().GetMouse().BindCamera(&Engine::GetInstance().free_camera_);

            /* bind camera to view */
            Engine::GetInstance().view_.BindCameraWithObjet(&Engine::GetInstance().free_camera_);
        } break;
        case CameraType::kFirstPerson:
            NOT_IMPLEMENTED;
            break;
        case CameraType::kThirdPerson:
            NOT_IMPLEMENTED;
            break;
        default:
            R_ASSERT(false && "Unknown camera type")
    }
}

void LibGcp::EngineBase::OnDefaultShaderChanged_(const uint64_t new_value)
{
    if (new_value == Engine::GetInstance().default_shader_->GetInstanceID()) {
        return;
    }

    /* correct value if necessary */
    if (new_value >= Shader::GetInstanceCount()) {
        SettingsMgr::GetInstance().SetSetting<Setting::kBaseShader>(Shader::GetInstanceCount() - 1);
        return;
    }

    Engine::GetInstance().default_shader_ = FindShaderWithId(new_value);
    R_ASSERT(Engine::GetInstance().default_shader_ && "Default shader not found");
}

void LibGcp::EngineBase::OnWordTimeChanged_(const uint64_t new_value)
{
    Engine::GetInstance().global_light_.UpdatePosition(new_value);
}

std::shared_ptr<LibGcp::Shader> LibGcp::EngineBase::FindShaderWithId(const uint64_t id)
{
    std::lock_guard lock(ResourceMgr::GetInstance().GetShaders().GetMutex());

    for (const auto &[_, shader] : ResourceMgr::GetInstance().GetShaders()) {
        if (shader->GetInstanceID() == id) {
            return shader;
        }
    }

    return nullptr;
}
