#include <libcgp/mgr/settings_mgr.hpp>
#include <libcgp/window/mouse.hpp>

#include <algorithm>

void LibGcp::Mouse::Move(const double x_pos, const double y_pos) noexcept
{
    const double sensitivity = SettingsMgr::GetInstance().GetSetting<double>(SettingsMgr::Setting::kMouseSensitivity);

    if (!is_enabled_) {
        return;
    }

    /* calculate offset */
    const double x_offset = (x_pos - last_x_) * sensitivity;
    const double y_offset = (last_y_ - y_pos) * sensitivity;

    /* save last position */
    last_x_ = x_pos;
    last_y_ = y_pos;

    if (camera_info_ == nullptr) {
        return;
    }

    /* update yaw and pith */
    camera_info_->yaw += x_offset;
    camera_info_->pitch += y_offset;

    /* clamp pitch */
    camera_info_->pitch = std::clamp(camera_info_->pitch, -89.0, 89.0);

    /* set front */
    camera_info_->ConvertYawPitchToVector();
}
