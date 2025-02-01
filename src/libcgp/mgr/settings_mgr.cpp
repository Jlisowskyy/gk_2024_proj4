#include <libcgp/mgr/settings_mgr.hpp>
#include <libcgp/settings.hpp>

LibGcp::SettingsMgr::SettingsMgr(const std::vector<std::tuple<Setting, SettingContainer>> &settings) noexcept
{
    TRACE("SettingsMgr::SettingsMgr()");

    /* load default settings */
    SetSetting(Setting::kCameraType, CameraType::kStatic);
    SetSetting<double>(Setting::kMouseSensitivity, 0.1);
    SetSetting(Setting::kClockTicking, true);

    /* load adjusted settings */
    for (const auto &[setting, container] : settings) {
        settings_[static_cast<size_t>(setting)] = container;
    }
}

LibGcp::SettingsMgr::~SettingsMgr() { TRACE("SettingsMgr::~SettingsMgr()"); }
