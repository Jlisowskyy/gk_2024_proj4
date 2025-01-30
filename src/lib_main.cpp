#include <libcgp/main.hpp>

#include <libcgp/engine/process_loop.hpp>
#include <libcgp/mgr/object_mgr.hpp>
#include <libcgp/mgr/resource_mgr.hpp>
#include <libcgp/mgr/settings_mgr.hpp>
#include <libcgp/window/window.hpp>

int LibGcp::RenderEngineMain(const SettingsMgr::setting_t &settings)
{
    /* initialize components */
    SettingsMgr::InitInstance(settings);
    ResourceMgr::InitInstance();
    ObjectMgr::InitInstance();

    Window::InitInstance().Init();

    /* render loop */
    Window::GetInstance().RunLoop(ProcessLoopApp);

    /* cleanup */
    Window::DeleteInstance();

    ObjectMgr::DeleteInstance();
    ResourceMgr::DeleteInstance();
    SettingsMgr::DeleteInstance();

    return 0;
}
