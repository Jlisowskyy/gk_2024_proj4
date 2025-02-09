#include <libcgp/mgr/settings_mgr.hpp>

LibGcp::SettingsMgrBase::SettingsMgrBase()
{
    TRACE("SettingsMgr::SettingsMgr()");
    LoadDefaultSettings();
}

LibGcp::SettingsMgrBase::~SettingsMgrBase() { TRACE("SettingsMgr::~SettingsMgr()"); }

void LibGcp::SettingsMgrBase::LoadDefaultSettings()
{
    SetSetting<Setting::kCameraType>(CameraType::kStatic);
    SetSetting<Setting::kMouseSensitivity, double>(0.1);
    SetSetting<Setting::kClockTicking>(true);
    SetSetting<Setting::kFreeCameraSpeed, double>(10.0);
    SetSetting<Setting::kBaseShader, uint64_t>(0);
    SetSetting<Setting::kCurrentWordTime, uint64_t>(1328819760);  // 09.02.2012 idk why this date
    SetSetting<Setting::kWordTimeCoef, double>(5.0);
}
