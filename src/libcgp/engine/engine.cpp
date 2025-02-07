#include <libcgp/engine/engine.hpp>

#include <libcgp/mgr/settings_mgr.hpp>
#include <libcgp/utils/macros.hpp>
#include <libcgp/window/window.hpp>

LibGcp::EngineBase::~EngineBase() { TRACE("EngineBase::~EngineBase()"); }

void LibGcp::EngineBase::Init() noexcept
{
    TRACE("EngineBase::Init()");

    /* connect with settings */
    SettingsMgr::GetInstance().AddListener(Setting::kCameraType, OnCameraTypeChanged_);

    /* Adjust camera type based on default settings - intentional cast */
    OnCameraTypeChanged_(SettingsMgr::GetInstance().GetSetting<uint64_t>(Setting::kCameraType));

    /* init flowing TEMP object */
    flowing_camera_.position = glm::vec3{};
    flowing_camera_.front    = glm::vec3(0.0f, 0.0f, -1.0f);
    flowing_camera_.up       = glm::vec3(0.0f, 1.0f, 0.0f);

    /* load default shader */
    const uint64_t default_id = SettingsMgr::GetInstance().GetSetting<uint64_t>(Setting::kBaseShader);
    R_ASSERT(default_id < Shader::GetInstanceCount() && "Default shader not found");
    ;

    default_shader_ = FindShaderWithId(default_id);
    R_ASSERT(Engine::GetInstance().default_shader_ && "Default shader not found");

    SettingsMgr::GetInstance().AddListener(Setting::kBaseShader, OnDefaultShaderChanged_);
}

void LibGcp::EngineBase::ProcessProgress(const long delta)
{
    ProcessInput_(delta);

    if (SettingsMgr::GetInstance().GetSetting<Setting::kClockTicking, bool>()) {
        ProcessDynamicObjects_(delta);
    }

    if (SettingsMgr::GetInstance().GetSetting<Setting::kCameraType, CameraType>() == CameraType::kFree) {
        ProcessFreeCameraMovement_(delta);
    }

    view_.UpdateCameraPosition();

    /* Reset keys input for next frame */
    keys_.fill(0);
}

void LibGcp::EngineBase::ProcessInput_(const long delta)
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

void LibGcp::EngineBase::ProcessUserMovement_([[maybe_unused]] const long delta) {}

void LibGcp::EngineBase::ProcessFreeCameraMovement_(const long delta)
{
    const double free_cam_speed = SettingsMgr::GetInstance().GetSetting<Setting::kFreeCameraSpeed, double>();
    const double distance       = free_cam_speed * static_cast<double>(delta) / 1e+6;

    free_camera_.MoveFreeCamera(static_cast<float>(distance), keys_);
}

void LibGcp::EngineBase::ProcessDynamicObjects_(const long delta)
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
