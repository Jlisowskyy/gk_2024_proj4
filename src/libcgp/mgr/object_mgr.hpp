#ifndef MGR_OBJECT_MGR_HPP_
#define MGR_OBJECT_MGR_HPP_

#include <libcgp/defines.hpp>

#include <CxxUtils/singleton.hpp>

#include <cassert>

LIBGCP_DECL_START_
class ObjectMgr : public CxxUtils::Singleton<ObjectMgr>
{
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

    // ---------------------------------
    // Class implementation methods
    // ---------------------------------

    protected:
    // ------------------------------
    // Class fields
    // ------------------------------
};

LIBGCP_DECL_END_

#endif  // MGR_OBJECT_MGR_HPP_
