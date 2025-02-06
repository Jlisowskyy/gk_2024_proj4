#ifndef MGR_OBJECT_MGR_HPP_
#define MGR_OBJECT_MGR_HPP_

#include <libcgp/defines.hpp>

#include <libcgp/intf.hpp>
#include <libcgp/primitives/shader.hpp>
#include <libcgp/primitives/static_object.hpp>

#include <CxxUtils/data_types/extended_map.hpp>
#include <CxxUtils/static_singleton.hpp>

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
    ObjectMgrBase() = default;
    ~ObjectMgrBase() override;

    // ------------------------------
    // Class interaction
    // ------------------------------

    void Init(const static_objects_t& static_objects, const dynamic_objects_t& dynamic_objects);

    void DrawStaticObjects(Shader& shader);

    void ProcessProgress(long delta_time_micros);

    NDSCRD FAST_CALL const std::vector<StaticObject>& GetStaticObjects() const { return static_objects_; }

    NDSCRD FAST_CALL StaticObject& GetStaticObject(const size_t idx) { return static_objects_[idx]; }

    FAST_CALL void AddStaticObject(const StaticObjectSpec& spec) { CreateStaticObject_(spec); }

    // ---------------------------------
    // Class implementation methods
    // ---------------------------------

    protected:
    void CreateStaticObject_(const StaticObjectSpec& spec);

    void CreateDynamicObject_(const DynamicObjectSpec& spec);

    // ------------------------------
    // Class fields
    // ------------------------------

    std::vector<StaticObject> static_objects_;
};

using ObjectMgr = CxxUtils::StaticSingleton<ObjectMgrBase>;

LIBGCP_DECL_END_

#endif  // MGR_OBJECT_MGR_HPP_
