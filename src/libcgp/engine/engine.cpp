#include <libcgp/engine/engine.hpp>

#include <libcgp/mgr/settings_mgr.hpp>
#include <libcgp/utils/macros.hpp>

LibGcp::Engine::~Engine() { TRACE("Engine::~Engine()"); }

void LibGcp::Engine::Init() noexcept
{
    TRACE("Engine::Init()");

    /* connect with settings */
    SettingsMgr::GetInstance().AddListener(SettingsMgr::Setting::kCameraType, OnCameraTypeChanged_);

    /* Adjust camera type based on default settings */
    OnCameraTypeChanged_(SettingsMgr::GetInstance().GetSetting<uint64_t>(SettingsMgr::Setting::kCameraType));
}

using key_action_t = void (LibGcp::Engine::*)(const int);
void LibGcp::Engine::ProcessProgress(const long delta)
{
    ProcessInput_(delta);

    ProcessDynamicObjects_(delta);

    view_.UpdateCameraPosition();
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

    /* process movement */
    ProcessUserMovement_(delta);

    keys_.fill(0);
}

void LibGcp::Engine::ProcessUserMovement_(const long delta)
{
    /* TODO: temporary, bind object movement to camera movement later */
    if (view_.GetCameraType() == CameraType::kFollow) {
        const float radius = 30.0f;
        const float camX   = sin(glfwGetTime()) * radius;
        const float camZ   = cos(glfwGetTime()) * radius;
        flow_position_     = glm::vec3(camX, 0.0f, camZ);

        return;
    }

    if (view_.GetCameraType() == CameraType::kFree) {
        ProcessFreeCameraMovement_(delta);
        return;
    }
}

void LibGcp::Engine::ProcessFreeCameraMovement_(const long delta) {}

void LibGcp::Engine::ProcessDynamicObjects_(const long delta) {}

void LibGcp::Engine::OnCameraTypeChanged_(const uint64_t new_value)
{
    TRACE("Camera type changed to " << new_value);

    GetInstance().view_.ChangeCameraType(static_cast<CameraType>(new_value));

    /* TODO: Bind proper object here */
    switch (static_cast<CameraType>(new_value)) {
        case CameraType::kStatic: {
            GetInstance().view_.BindCameraWithObjet(nullptr, nullptr);
        } break;
        case CameraType::kFollow: {
            GetInstance().view_.BindCameraWithObjet(&GetInstance().flow_position_, &GetInstance().flow_direction_);
        } break;
        case CameraType::kFree: {
            GetInstance().view_.BindCameraWithObjet(
                &GetInstance().free_camera_position_, &GetInstance().free_camera_front_
            );
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
