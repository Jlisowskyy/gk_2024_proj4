#include <libcgp/mgr/settings_mgr.hpp>
#include <libcgp/window/mouse.hpp>

#include <algorithm>

void LibGcp::Mouse::Move(const double x_pos, const double y_pos) noexcept
{
    const double sensitivity = SettingsMgr::GetInstance().GetSetting<double>(SettingsMgr::Setting::kMouseSensitivity);

    /* calculate offset */
    const double x_offset = (x_pos - last_x_) * sensitivity;
    const double y_offset = (last_y_ - y_pos) * sensitivity;

    /* update yaw and pith */
    yaw_ += x_offset;
    last_x_ = x_pos;
    pitch_ += y_offset;
    last_y_ = y_pos;

    /* clamp pitch */
    pitch_ = std::clamp(pitch_, -89.0, 89.0);

    /* set front */
    front_ = glm::normalize(glm::vec3(
        cos(glm::radians(yaw_)) * cos(glm::radians(pitch_)), sin(glm::radians(pitch_)),
        sin(glm::radians(yaw_)) * cos(glm::radians(pitch_))
    ));
}
