#ifndef ENGINE_LIGHT_MGR_HPP_
#define ENGINE_LIGHT_MGR_HPP_

#include <libcgp/defines.hpp>
#include <libcgp/intf.hpp>

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
