#ifndef MGR_OBJECT_MGR_HPP_
#define MGR_OBJECT_MGR_HPP_

#include <libcgp/defines.hpp>

#include <libcgp/intf.hpp>
#include <libcgp/primitives/static_object.hpp>

#include <CxxUtils/data_types/extended_vector.hpp>
#include <CxxUtils/static_singleton.hpp>

#include <mutex>
#include <vector>

LIBGCP_DECL_START_
class ObjectMgrBase : public CxxUtils::StaticSingletonHelper
{
    // ------------------------------
    // Internal public types
    // ------------------------------

    static constexpr size_t kDefaultStorageSize = static_cast<size_t>(2 * 16384);

    // ------------------------------
    // Object creation
    // ------------------------------

    public:
    ObjectMgrBase();

    ~ObjectMgrBase() override;

    // ------------------------------
    // Class interaction
    // ------------------------------

    void LoadObjectsFromScene(const Scene &scene);

    void DrawStaticObjects(Shader &shader, RenderPass pass) const;

    void ProcessProgress(long delta_time_micros);

    NDSCRD FAST_CALL CxxUtils::ExtendedVector<StaticObject> &GetStaticObjects() { return static_objects_; }

    FAST_CALL void AddStaticObject(const StaticObjectSpec &spec) { CreateStaticObject_(spec); }

    void RemoveStaticObject(uint64_t ident);

    // ---------------------------------
    // Class implementation methods
    // ---------------------------------

    protected:
    void CreateStaticObject_(const StaticObjectSpec &spec);

    void CreateDynamicObject_(const DynamicObjectSpec &spec);

    // ------------------------------
    // Class fields
    // ------------------------------

    CxxUtils::ExtendedVector<StaticObject> static_objects_;
};

using ObjectMgr = CxxUtils::StaticSingleton<ObjectMgrBase>;

LIBGCP_DECL_END_

#endif  // MGR_OBJECT_MGR_HPP_
