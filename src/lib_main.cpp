#include <libcgp/main.hpp>

#include <libcgp/engine/engine.hpp>
#include <libcgp/engine/process_loop.hpp>
#include <libcgp/mgr/object_mgr.hpp>
#include <libcgp/mgr/resource_mgr.hpp>
#include <libcgp/mgr/settings_mgr.hpp>
#include <libcgp/window/window.hpp>

#include <string>

int LibGcp::RenderEngineMain(
    const SettingsMgr::setting_t &settings, const ResourceMgr::resource_t &resources,
    const ObjectMgr::dynamic_objects_t &dynamic_objects, const ObjectMgr::static_objects_t &static_objects,
    const std::string &default_shader_name
)
{
    /* initialize components */
    SettingsMgr::InitInstance(settings);
    Window::InitInstance().Init();
    ResourceMgr::InitInstance().Init(resources);
    ObjectMgr::InitInstance().Init(static_objects, dynamic_objects);
    Engine::InitInstance();

    /* render loop */
    ProcessLoopApp(default_shader_name);

    /* cleanup */
    Window::DeleteInstance();

    Engine::DeleteInstance();
    ObjectMgr::DeleteInstance();
    ResourceMgr::DeleteInstance();
    SettingsMgr::DeleteInstance();

    return 0;
}
