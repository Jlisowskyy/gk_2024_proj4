#include <libcgp/engine/global_light.hpp>
#include <libcgp/engine/word_time.hpp>
#include <libcgp/intf.hpp>
#include <libcgp/utils/macros.hpp>

#include <cassert>
#include <cstdint>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

LibGcp::GlobalLights::GlobalLight::GlobalLight(const GlobalLightSpec &spec) noexcept : spec_(spec)
{
    assert(spec.down_time < WordTime::kSecondsInDay && "Down time is out of range");
    assert(spec.rise_time < WordTime::kSecondsInDay && "Rise time is out of range");

    if (spec_.is_moving) {
        UpdatePosition(WordTime::GetDayTimeSeconds());
    }
}

void LibGcp::GlobalLights::GlobalLight::UpdatePosition(const uint64_t time)
{
    assert(spec_.is_moving);

    if (IsBelowHorizon(time)) {
        /* position serves as a light direction here */
        spec_.light_info.position = {};
        return;
    }

    const double day_angle   = GetDayAngle(time);
    const auto rot_day_angle = glm::rotate(glm::mat4(1.0F), static_cast<float>(day_angle), {0.0F, 0.0F, 1.0F});
    const auto rot_light_angle =
        glm::rotate(glm::mat4(1.0F), static_cast<float>(spec_.angle * 2 * M_PI), {0.0F, 1.0F, 0.0F});

    glm::vec3 light_vec = {1.0F, 0.0F, 0.0F};
    light_vec           = -glm::vec3(rot_day_angle * rot_light_angle * glm::vec4(light_vec, 1.0F));

    spec_.light_info.position = light_vec;
}

bool LibGcp::GlobalLights::GlobalLight::IsBelowHorizon(const uint64_t time) const
{
    return spec_.rise_time > spec_.down_time ? time > spec_.rise_time || time < spec_.down_time
                                             : time > spec_.rise_time && time < spec_.down_time;
}

double LibGcp::GlobalLights::GlobalLight::GetDayAngle(const uint64_t time) const
{
    if (spec_.rise_time > spec_.down_time) {
        const auto len         = static_cast<double>(WordTime::kSecondsInDay - spec_.rise_time + spec_.down_time);
        const auto time_passed = static_cast<double>(
            time < spec_.rise_time ? WordTime::kSecondsInDay - spec_.rise_time + time : time - spec_.rise_time
        );

        return M_PI * (time_passed / len);
    }

    const auto len         = static_cast<double>(spec_.down_time - spec_.rise_time);
    const auto time_passed = static_cast<double>(time - spec_.rise_time);

    return M_PI * (time_passed / len);
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
        light.UpdatePosition(time);
    }
}
