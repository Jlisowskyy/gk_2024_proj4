#ifndef MGR_SETTINGS_MGR_HPP_
#define MGR_SETTINGS_MGR_HPP_

#include <libcgp/defines.hpp>

#include <CxxUtils/singleton.hpp>

#include <cassert>

LIBGCP_DECL_START_

class SettingsMgr : public CxxUtils::Singleton<SettingsMgr>
{
    protected:
    // ------------------------------
    // Object creation
    // ------------------------------

    SettingsMgr() = default;

    public:
    ~SettingsMgr();

    FAST_CALL static SettingsMgr& InitInstance()
    {
        assert(!IsInited());

        Singleton::InitInstance(new SettingsMgr());
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

#endif  // MGR_SETTINGS_MGR_HPP_
