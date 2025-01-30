#include <libcgp/mgr/resource_mgr.hpp>
#include <libcgp/mgr/settings_mgr.hpp>
#include <libcgp/window/window.hpp>

extern int RenderEngineMain()
{
    /* initialize components */
    LibGcp::ResourceMgr::InitInstance();
    LibGcp::SettingsMgr::InitInstance();
    LibGcp::Window::InitInstance().Init();

    /* render loop */
    LibGcp::Window::GetInstance().RunLoop();

    /* cleanup */
    LibGcp::Window::DeleteInstance();
    LibGcp::ResourceMgr::DeleteInstance();
    LibGcp::SettingsMgr::DeleteInstance();

    return 0;
}
