#include <libcgp/engine/engine.hpp>

#include <libcgp/mgr/settings_mgr.hpp>
#include <libcgp/utils/macros.hpp>
#include <libcgp/window/window.hpp>

LibGcp::Engine::~Engine() { TRACE("Engine::~Engine()"); }

void LibGcp::Engine::Init() noexcept
{
    TRACE("Engine::Init()");

    /* connect with settings */
    SettingsMgr::GetInstance().AddListener(SettingsMgr::Setting::kCameraType, OnCameraTypeChanged_);

    /* Adjust camera type based on default settings */
    OnCameraTypeChanged_(SettingsMgr::GetInstance().GetSetting<uint64_t>(SettingsMgr::Setting::kCameraType));

    /* init flowing TEMP object */
    flowing_camera_.position = glm::vec3{};
    flowing_camera_.front    = glm::vec3(0.0f, 0.0f, -1.0f);
    flowing_camera_.up       = glm::vec3(0.0f, 1.0f, 0.0f);
}

void LibGcp::Engine::ProcessProgress(const long delta)
{
    ProcessInput_(delta);

    if (SettingsMgr::GetInstance().GetSetting<long>(SettingsMgr::Setting::kClockTicking)) {
        ProcessDynamicObjects_(delta);
    }

    if (SettingsMgr::GetInstance().GetSetting<CameraType>(SettingsMgr::Setting::kCameraType) == CameraType::kFree) {
        TRACE("LOL");
        ProcessFreeCameraMovement_(delta);
    }

    view_.UpdateCameraPosition();

    /* Reset keys input for next frame */
    keys_.fill(0);
}

void LibGcp::Engine::ProcessInput_(const long delta)
{
    /* first process camera change */
    if (keys_[GLFW_KEY_F1]) {
        SettingsMgr::GetInstance().SetSetting(SettingsMgr::Setting::kCameraType, CameraType::kStatic);
    } else if (keys_[GLFW_KEY_F2]) {
        SettingsMgr::GetInstance().SetSetting(SettingsMgr::Setting::kCameraType, CameraType::kFollow);
    } else if (keys_[GLFW_KEY_F3]) {
        SettingsMgr::GetInstance().SetSetting(SettingsMgr::Setting::kCameraType, CameraType::kFree);
    } else if (keys_[GLFW_KEY_F4]) {
        SettingsMgr::GetInstance().SetSetting(SettingsMgr::Setting::kCameraType, CameraType::kFirstPerson);
    } else if (keys_[GLFW_KEY_F5]) {
        SettingsMgr::GetInstance().SetSetting(SettingsMgr::Setting::kCameraType, CameraType::kThirdPerson);
    }

    /* process overlay changes */
    if (keys_[GLFW_KEY_F12]) {
        Window::GetInstance().SwitchDebugOverlay();
    }

    /* process movement */
    const long is_clock_enabled = SettingsMgr::GetInstance().GetSetting<long>(SettingsMgr::Setting::kClockTicking);
    ProcessUserMovement_(is_clock_enabled * delta);
}

void LibGcp::Engine::ProcessUserMovement_(const long delta) {}

void LibGcp::Engine::ProcessFreeCameraMovement_(const long delta)
{
    static constexpr double kFreeCamSpeed = 10.0;

    const double distance = kFreeCamSpeed * static_cast<double>(delta) / 1e+6;

    free_camera_.MoveFreeCamera(static_cast<float>(distance), keys_);
}

void LibGcp::Engine::ProcessDynamicObjects_(const long delta)
{
    /* TODO: temporary, bind object movement to camera movement later */
    const double radius = 30.0f;
    flow_count_ += static_cast<double>(delta) / 1e+6;
    const float camX = sin(flow_count_) * radius;
    const float camZ = cos(flow_count_) * radius;

    flowing_camera_.position = glm::vec3(camX, 0.0f, camZ);
    flowing_camera_.front    = -flowing_camera_.position;
}

void LibGcp::Engine::OnCameraTypeChanged_(const uint64_t new_value)
{
    TRACE("Camera type changed to " << new_value);

    /* TODO: Bind proper object here */
    switch (static_cast<CameraType>(new_value)) {
        case CameraType::kStatic: {
            Window::GetInstance().GetMouse().BindCamera(nullptr);
            GetInstance().view_.BindCameraWithObjet(&GetInstance().static_camera_);
        } break;
        case CameraType::kFollow: {
            Window::GetInstance().GetMouse().BindCamera(nullptr);
            GetInstance().view_.BindCameraWithObjet(&GetInstance().flowing_camera_);
        } break;
        case CameraType::kFree: {
            /* preserve old camera */
            GetInstance().free_camera_ = GetInstance().view_.GetBindObject();

            /* ensure yaw pitch is set and ready */
            GetInstance().free_camera_.ConvertVectorToYawPitch();

            /* bind camera to mouse */
            Window::GetInstance().GetMouse().BindCamera(&GetInstance().free_camera_);

            /* bind camera to view */
            GetInstance().view_.BindCameraWithObjet(&GetInstance().free_camera_);
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
