#include <libcgp/main.hpp>

#include <libcgp/engine/engine.hpp>
#include <libcgp/engine/process_loop.hpp>
#include <libcgp/mgr/object_mgr.hpp>
#include <libcgp/mgr/resource_mgr.hpp>
#include <libcgp/mgr/settings_mgr.hpp>
#include <libcgp/window/window.hpp>

int LibGcp::RenderEngineMain(const Scene& scene)
{
    /* initialize components */
    SettingsMgr::InitInstance();
    Window::InitInstance().Init();
    ResourceMgr::InitInstance();
    ObjectMgr::InitInstance();
    Engine::InitInstance().Init(scene);

    /* render loop */
    ProcessLoopApp();

    /* cleanup */
    Window::DeleteInstance();

    Engine::DeleteInstance();
    ObjectMgr::DeleteInstance();
    ResourceMgr::DeleteInstance();
    SettingsMgr::DeleteInstance();

    return 0;
}
