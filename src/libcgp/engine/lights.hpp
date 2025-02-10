#ifndef ENGINE_LIGHTS_HPP_
#define ENGINE_LIGHTS_HPP_

#include <libcgp/defines.hpp>
#include <libcgp/intf.hpp>

#include <string>

LIBGCP_DECL_START_
struct PointLight {
    explicit PointLight(const PointLightSpec &spec) : light_info(spec.light_info), point_light(spec.point_light) {}

    FAST_CALL SceneSerialized::PointLightSerialized Serialize(const uint64_t model_id) const
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

    FAST_CALL SceneSerialized::SpotLightSerialized Serialize(const uint64_t model_id) const
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

LIBGCP_DECL_END_

#endif  // ENGINE_LIGHTS_HPP_
