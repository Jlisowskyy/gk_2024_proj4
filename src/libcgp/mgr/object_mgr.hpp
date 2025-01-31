#ifndef MGR_OBJECT_MGR_HPP_
#define MGR_OBJECT_MGR_HPP_

#include <libcgp/defines.hpp>

#include <libcgp/primitives/shader.hpp>
#include <libcgp/primitives/static_object.hpp>

#include <CxxUtils/singleton.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <cassert>
#include <string>
#include <vector>

LIBGCP_DECL_START_
class ObjectMgr : public CxxUtils::Singleton<ObjectMgr>
{
    // ------------------------------
    // Internal public types
    // ------------------------------

    static constexpr size_t kDefaultStorageSize = static_cast<size_t>(2 * 16384);

    public:
    struct DynamicObjectSpec {
        int xd;
    };

    using static_objects_t  = std::vector<StaticObjectSpec>;
    using dynamic_objects_t = std::vector<DynamicObjectSpec>;

    // ------------------------------
    // Object creation
    // ------------------------------

    protected:
    ObjectMgr() = default;

    public:
    ~ObjectMgr();

    FAST_CALL static ObjectMgr& InitInstance()
    {
        assert(!IsInited());

        Singleton::InitInstance(new ObjectMgr());
        return GetInstance();
    }

    // ------------------------------
    // Class interaction
    // ------------------------------

    void Init(const static_objects_t& static_objects, const dynamic_objects_t& dynamic_objects);

    void DrawStaticObjects(Shader& shader);

    void ProcessProgress(long delta_time_micros);

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

LIBGCP_DECL_END_

#endif  // MGR_OBJECT_MGR_HPP_
