#ifndef ENGINE_GLOBAL_LIGHT_HPP_
#define ENGINE_GLOBAL_LIGHT_HPP_

#include <libcgp/defines.hpp>
#include <libcgp/engine/word_time.hpp>
#include <libcgp/intf.hpp>

#include <cstdint>
#include <libcgp/utils/macros.hpp>
#include <vector>

/**
 * TODO:
 * - configurable global lights
 */

LIBGCP_DECL_START_
/* Forward declarations */
class Shader;

class GlobalLights
{
    // ------------------------------
    // Class internals
    // ------------------------------

    struct GlobalLight {
        explicit GlobalLight(const GlobalLightSpec &spec) noexcept;

        void UpdatePosition(uint64_t time);

        NDSCRD bool IsBelowHorizon(uint64_t time) const;

        NDSCRD double GetDayAngleAndUpdateIntensity(uint64_t time);

        void PrepareLight(Shader &shader, size_t idx);

        GlobalLightSpec spec_;
    };

    public:
    static constexpr size_t kMaxLights = 8;

    // ------------------------------
    // Object creation
    // ------------------------------

    // ------------------------------
    // Object interaction
    // ------------------------------

    FAST_CALL void AddLight(const GlobalLightSpec &spec)
    {
        lights_.emplace_back(spec);
        R_ASSERT(lights_.size() < kMaxLights && "Too many global lights");
    }

    FAST_CALL void RemoveLight(const size_t idx) { lights_.erase(lights_.begin() + idx); }

    NDSCRD GlobalLightSpec &GetLight(const size_t idx) { return lights_[idx].spec_; }

    NDSCRD size_t GetLightsCount() const { return lights_.size(); }

    void LoadLights(const std::vector<GlobalLightSpec> &lights);

    void UpdatePosition(uint64_t time);

    void PrepareLights(Shader &shader);

    // ---------------------------------
    // Class implementation methods
    // ---------------------------------

    protected:
    // ------------------------------
    // Class fields
    // ------------------------------

    std::vector<GlobalLight> lights_{};
};

static inline GlobalLightSpec kDefaultGlobalLight{
    .light_info =
        {
                     .ambient  = {0.1F, 0.1F, 0.1F},
                     .diffuse  = {0.5F, 0.5F, 0.5F},
                     .specular = {1.0F, 1.0F, 1.0F},
                     },
    .is_moving = true,
    .rise_time = WordTime::ConvertToSeconds(6, 0, 0),
    .down_time = WordTime::ConvertToSeconds(22, 30, 0),
    .angle     = 0.25,
};

LIBGCP_DECL_END_

#endif  // ENGINE_GLOBAL_LIGHT_HPP_
