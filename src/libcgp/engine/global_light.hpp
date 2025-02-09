#ifndef ENGINE_GLOBAL_LIGHT_HPP_
#define ENGINE_GLOBAL_LIGHT_HPP_

#include <libcgp/defines.hpp>
#include <libcgp/intf.hpp>

#include <cstdint>
#include <vector>

/**
 * TODO:
 * - configurable global lights
 */

LIBGCP_DECL_START_
class GlobalLights
{
    // ------------------------------
    // Class internals
    // ------------------------------

    struct GlobalLight {
        explicit GlobalLight(const GlobalLightSpec &spec) noexcept;

        void UpdatePosition(uint64_t time);
        NDSCRD bool IsBelowHorizon(uint64_t time) const;
        NDSCRD double GetDayAngle(uint64_t time) const;

        protected:
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

    void LoadLights(const std::vector<GlobalLightSpec> &lights);

    void UpdatePosition(uint64_t time);

    // ---------------------------------
    // Class implementation methods
    // ---------------------------------

    protected:
    // ------------------------------
    // Class fields
    // ------------------------------

    std::vector<GlobalLight> lights_{};
};

LIBGCP_DECL_END_

#endif  // ENGINE_GLOBAL_LIGHT_HPP_
