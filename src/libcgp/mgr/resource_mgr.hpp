#ifndef MGR_RESOURCE_MGR_HPP_
#define MGR_RESOURCE_MGR_HPP_

#include <libcgp/defines.hpp>

#include <CxxUtils/singleton.hpp>

#include <cassert>

LIBGCP_DECL_START_
class ResourceMgr : public CxxUtils::Singleton<ResourceMgr>
{
    // ------------------------------
    // Object creation
    // ------------------------------

    protected:
    ResourceMgr() = default;

    public:
    ~ResourceMgr();

    FAST_CALL static ResourceMgr& InitInstance()
    {
        assert(!IsInited());

        Singleton::InitInstance(new ResourceMgr());
        return GetInstance();
    }

    // ------------------------------
    // Class interaction
    // ------------------------------

    // ---------------------------------
    // Class implementation methods
    // ---------------------------------

    protected:
    // ------------------------------
    // Class fields
    // ------------------------------
};

LIBGCP_DECL_END_

#endif  // MGR_RESOURCE_MGR_HPP_
