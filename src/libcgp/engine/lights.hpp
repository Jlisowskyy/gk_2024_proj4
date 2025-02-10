#ifndef ENGINE_LIGHTS_HPP_
#define ENGINE_LIGHTS_HPP_

#include <libcgp/defines.hpp>
#include <libcgp/intf.hpp>

#include <string>

LIBGCP_DECL_START_
struct PointLight {
    explicit PointLight(const PointLightSpec &spec) : light_info(spec.light_info), point_light(spec.point_light) {}

    FAST_CALL constexpr SceneSerialized::PointLightSerialized Serialize(const uint64_t model_id) const
    {
        return {
            .model       = model_id,
            .light_info  = light_info,
            .point_light = point_light,
        };
    }

    LightInfo light_info{};
    PointLightInfo point_light{};
};

struct SpotLight : SpotLightSpec {
    explicit SpotLight(const SpotLightSpec &spec) : light_info(spec.light_info), spot_light(spec.spot_light) {}

    FAST_CALL constexpr SceneSerialized::SpotLightSerialized Serialize(const uint64_t model_id) const
    {
        return {
            .model      = model_id,
            .light_info = light_info,
            .spot_light = spot_light,
        };
    }

    LightInfo light_info{};
    SpotLightInfo spot_light{};
};

static inline PointLight kDefaultPointLight{
    PointLightSpec{
                   .model_name = "",
                   .light_info =
            {
                .position  = {0.0f, 5.0f, 0.0f},
                .ambient   = {0.2f, 0.2f, 0.2f},
                .diffuse   = {0.5f, 0.5f, 0.5f},
                .specular  = {1.0f, 1.0f, 1.0f},
                .intensity = 1.0,
            }, .point_light =
            {
                .constant  = 1.0,
                .linear    = 0.09,
                .quadratic = 0.032,
            }
    }
};

static inline SpotLight kDefaultSpotLight{
    SpotLightSpec{
                  .model_name = "",
                  .light_info =
            {
                .position  = {0.0f, 10.0f, 0.0f},
                .ambient   = {0.1f, 0.1f, 0.1f},
                .diffuse   = {0.8f, 0.8f, 0.8f},
                .specular  = {1.0f, 1.0f, 1.0f},
                .intensity = 1.0,
            }, .spot_light =
            {
                .direction     = {0.0f, -1.0f, 0.0f},
                .constant      = 1.0,
                .linear        = 0.09,
                .quadratic     = 0.032,
                .cut_off       = glm::cos(glm::radians(12.5f)),
                .outer_cut_off = glm::cos(glm::radians(17.5f)),
            }
    }
};

LIBGCP_DECL_END_

#endif  // ENGINE_LIGHTS_HPP_
