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
    FAST_CALL static void AddLight(Model &model, const T &light)
    {
        model.GetLights().push_back(light);
        R_ASSERT(model.GetLights().size<T>() < kMaxLightPerObject && "Too many lights");
    }

    // ---------------------------------
    // Class implementation methods
    // ---------------------------------

    protected:
    // ------------------------------
    // Class fields
    // ------------------------------
};

LIBGCP_DECL_END_

#endif  // ENGINE_LIGHT_MGR_HPP_
