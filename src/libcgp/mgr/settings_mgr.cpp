#include <libcgp/mgr/settings_mgr.hpp>

LibGcp::SettingsMgrBase::SettingsMgrBase(const std::vector<std::tuple<Setting, SettingContainer>> &settings) noexcept
{
    TRACE("SettingsMgr::SettingsMgr()");

    /* load default settings */
    SetSetting<Setting::kCameraType>(CameraType::kStatic);
    SetSetting<Setting::kMouseSensitivity, double>(0.1);
    SetSetting<Setting::kClockTicking>(true);
    SetSetting<Setting::kFreeCameraSpeed, double>(10.0);

    /* load adjusted settings */
    for (const auto &[setting, container] : settings) {
        settings_[static_cast<size_t>(setting)] = container;
    }
}

LibGcp::SettingsMgrBase::~SettingsMgrBase() { TRACE("SettingsMgr::~SettingsMgr()"); }
