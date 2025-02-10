#include <libcgp/engine/word_time.hpp>
#include <libcgp/intf.hpp>
#include <libcgp/mgr/settings_mgr.hpp>

#include <cstdint>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>

void LibGcp::WordTime::UpdateTime(const uint64_t delta_time) noexcept
{
    const double scale = SettingsMgr::GetInstance().GetSetting<Setting::kWordTimeCoef, double>();
    tick_remainder_ += static_cast<uint64_t>(static_cast<double>(delta_time) * scale);

    const uint64_t seconds = tick_remainder_ / kTicksToSecond;
    tick_remainder_ -= seconds * kTicksToSecond;

    const uint64_t new_time = SettingsMgr::GetInstance().GetSetting<Setting::kCurrentWordTime, uint64_t>() + seconds;
    SettingsMgr::GetInstance().SetSetting<Setting::kCurrentWordTime>(new_time);
}

std::string LibGcp::WordTime::GetTime() noexcept
{
    const uint64_t time = SettingsMgr::GetInstance().GetSetting<Setting::kCurrentWordTime, uint64_t>();
    const auto time_t   = static_cast<std::time_t>(time);

    tm time_info;
    gmtime_r(&time_t, &time_info);

    std::stringstream ss;
    ss << std::put_time(&time_info, "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

uint64_t LibGcp::WordTime::GetDayTimeSeconds(const uint64_t time)
{
    const auto time_t = static_cast<std::time_t>(time);

    tm timeInfo;
    gmtime_r(&time_t, &timeInfo);

    return timeInfo.tm_hour * 3600 + timeInfo.tm_min * 60 + timeInfo.tm_sec;
}
