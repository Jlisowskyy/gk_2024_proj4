#include <libcgp/engine/global_light.hpp>
#include <libcgp/engine/word_time.hpp>
#include <libcgp/intf.hpp>
#include <libcgp/mgr/settings_mgr.hpp>
#include <libcgp/primitives/shader.hpp>
#include <libcgp/utils/macros.hpp>

#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

LibGcp::GlobalLights::GlobalLight::GlobalLight(const GlobalLightSpec &spec) noexcept : spec_(spec)
{
    assert(spec.down_time < WordTime::kSecondsInDay && "Down time is out of range");
    assert(spec.rise_time < WordTime::kSecondsInDay && "Rise time is out of range");

    if (spec_.is_moving) {
        UpdatePosition(
            WordTime::GetDayTimeSeconds(SettingsMgr::GetInstance().GetSetting<Setting::kCurrentWordTime, uint64_t>())
        );
    }
}

void LibGcp::GlobalLights::GlobalLight::UpdatePosition(const uint64_t time)
{
    assert(spec_.is_moving);

    if (IsBelowHorizon(time)) {
        /* position serves as a light direction here */
        spec_.light_info.intensity = 0.0F;
        return;
    }

    const double day_angle   = GetDayAngleAndUpdateIntensity(time);
    const auto rot_day_angle = rotate(glm::mat4(1.0F), static_cast<float>(day_angle), {0.0F, 0.0F, 1.0F});
    const auto rot_light_angle =
        glm::rotate(glm::mat4(1.0F), static_cast<float>(spec_.angle * 2 * M_PI), {0.0F, 1.0F, 0.0F});

    glm::vec3 light_vec = {1.0F, 0.0F, 0.0F};
    light_vec           = -glm::vec3(rot_day_angle * rot_light_angle * glm::vec4(light_vec, 1.0F));

    spec_.light_info.position = -light_vec;
}

bool LibGcp::GlobalLights::GlobalLight::IsBelowHorizon(const uint64_t time) const
{
    return !(
        spec_.rise_time > spec_.down_time ? time > spec_.rise_time || time < spec_.down_time
                                          : time > spec_.rise_time && time < spec_.down_time
    );
}

double LibGcp::GlobalLights::GlobalLight::GetDayAngleAndUpdateIntensity(const uint64_t time)
{
    if (spec_.rise_time > spec_.down_time) {
        const auto len         = static_cast<double>(WordTime::kSecondsInDay - spec_.rise_time + spec_.down_time);
        const auto time_passed = static_cast<double>(
            time < spec_.rise_time ? WordTime::kSecondsInDay - spec_.rise_time + time : time - spec_.rise_time
        );

        const auto mid_point       = len / 2;
        spec_.light_info.intensity = 1.0F - (std::abs(time_passed - mid_point) / mid_point);

        return M_PI * (time_passed / len);
    }

    const auto len             = static_cast<double>(spec_.down_time - spec_.rise_time);
    const auto time_passed     = static_cast<double>(time - spec_.rise_time);
    const auto mid_point       = len / 2;
    spec_.light_info.intensity = 1.0F - (std::abs(time_passed - mid_point) / mid_point);

    return M_PI * (time_passed / len);
}

void LibGcp::GlobalLights::GlobalLight::PrepareLight(Shader &shader, const size_t idx)
{
    const std::string prefix = "un_lightning.global_lights[" + std::to_string(idx) + "].";

    shader.SetVec3Unsafe((prefix + "position").c_str(), spec_.light_info.position);
    shader.SetVec3Unsafe((prefix + "ambient").c_str(), spec_.light_info.ambient);
    shader.SetVec3Unsafe((prefix + "diffuse").c_str(), spec_.light_info.diffuse);
    shader.SetVec3Unsafe((prefix + "specular").c_str(), spec_.light_info.specular);
    shader.SetGLfloatUnsafe((prefix + "intensity").c_str(), spec_.light_info.intensity);
}

void LibGcp::GlobalLights::LoadLights(const std::vector<GlobalLightSpec> &lights)
{
    R_ASSERT(lights.size() < GlobalLights::kMaxLights && "Too many global lights");
    lights_.clear();

    for (const auto &light : lights) {
        lights_.emplace_back(light);
    }
}

void LibGcp::GlobalLights::UpdatePosition(const uint64_t time)
{
    for (auto &light : lights_) {
        if (!light.spec_.is_moving) {
            continue;
        }

        light.UpdatePosition(WordTime::GetDayTimeSeconds(time));
    }
}

void LibGcp::GlobalLights::PrepareLights(Shader &shader)
{
    shader.SetGLuintUnsafe("un_lightning.num_global_lights", static_cast<GLuint>(lights_.size()));

    for (size_t idx = 0; idx < lights_.size(); ++idx) {
        lights_[idx].PrepareLight(shader, idx);
    }
}
