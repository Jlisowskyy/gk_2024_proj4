#ifndef ENGINE_LIGHT_MGR_HPP_
#define ENGINE_LIGHT_MGR_HPP_

#include <libcgp/defines.hpp>
#include <libcgp/intf.hpp>
#include <libcgp/utils/macros.hpp>

#include <libcgp/primitives/model.hpp>

LIBGCP_DECL_START_

/* Forward declarations */
class Shader;

class LightMgr
{
    public:
    // ------------------------------
    // Class internals
    // ------------------------------

    public:
    // ------------------------------
    // Object creation
    // ------------------------------

    // ------------------------------
    // Object interaction
    // ------------------------------

    void LoadLightsFromScene(const Scene &scene);

    void PrepareLights(Shader &shader) const;

    template <typename T>
    void AddLight(Model &model, const T &light)
    {
        model.GetLights().push_back(light);

        if constexpr (std::is_same_v<T, SpotLight>) {
            spot_light_count_++;
        } else if constexpr (std::is_same_v<T, PointLight>) {
            point_light_count_++;
        }

        R_ASSERT(point_light_count_ <= kMaxTypeLightObjects && "Too many point lights");
        R_ASSERT(spot_light_count_ <= kMaxTypeLightObjects && "Too many spot lights");
        R_ASSERT(point_light_count_ + spot_light_count_ <= kMaxLightObjects && "Too many lights");
    }

    // ---------------------------------
    // Class implementation methods
    // ---------------------------------

    protected:
    // ------------------------------
    // Class fields
    // ------------------------------

    uint16_t spot_light_count_  = 0;
    uint16_t point_light_count_ = 0;
};

LIBGCP_DECL_END_

#endif  // ENGINE_LIGHT_MGR_HPP_
