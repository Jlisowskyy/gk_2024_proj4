#include <libcgp/main.hpp>

#include <libcgp/engine/engine.hpp>
#include <libcgp/engine/process_loop.hpp>
#include <libcgp/mgr/object_mgr.hpp>
#include <libcgp/mgr/resource_mgr.hpp>
#include <libcgp/mgr/settings_mgr.hpp>
#include <libcgp/window/window.hpp>

#include <string>

int LibGcp::RenderEngineMain(const Scene& scene)
{
    /* initialize components */
    SettingsMgr::InitInstance(scene.settings);
    Window::InitInstance().Init();
    ResourceMgr::InitInstance().Init(scene.resources);
    ObjectMgr::InitInstance().Init(scene.static_object, scene.dynamic_objects);
    Engine::InitInstance().Init();

    /* render loop */
    ProcessLoopApp(scene.default_shader_name);

    /* cleanup */
    Window::DeleteInstance();

    Engine::DeleteInstance();
    ObjectMgr::DeleteInstance();
    ResourceMgr::DeleteInstance();
    SettingsMgr::DeleteInstance();

    return 0;
}
