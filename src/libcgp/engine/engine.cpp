#include <libcgp/engine/engine.hpp>

#include <libcgp/mgr/settings_mgr.hpp>

LibGcp::Engine::~Engine() { TRACE("Engine::~Engine()"); }

void LibGcp::Engine::Init() noexcept
{
    TRACE("Engine::Init()");

    SettingsMgr::GetInstance().AddListener(SettingsMgr::Setting::kCameraType, OnCameraTypeChanged);
}

void LibGcp::Engine::OnCameraTypeChanged(const uint64_t new_value)
{
    TRACE("Camera type changed to " << new_value);

    GetInstance().view_.ChangeCameraType(static_cast<CameraType>(new_value));
}
