#include <libcgp/main.hpp>

#include <libcgp/mgr/object_mgr.hpp>
#include <libcgp/mgr/resource_mgr.hpp>
#include <libcgp/mgr/settings_mgr.hpp>

static constexpr const char* kMapPath = "./models/csgo.glb";

int main()
{
    return LibGcp::RenderEngineMain(
        {
    },
        {LibGcp::ResourceMgr::ResourceSpec{
             .paths     = {"first_vertex_shader", "first_fragment_shader"},
             .type      = LibGcp::ResourceMgr::ResourceType::kShader,
             .load_type = LibGcp::ResourceMgr::LoadType::kMemory,
         },
         // LibGcp::ResourceMgr::ResourceSpec{
         //     .paths = {"./models/backpack/backpack.obj"},
         //     .type = LibGcp::ResourceMgr::ResourceType::kModel,
         //     .load_type = LibGcp::ResourceMgr::LoadType::kExternal,
         // },
         LibGcp::ResourceMgr::ResourceSpec{
             .paths     = {kMapPath},
             .type      = LibGcp::ResourceMgr::ResourceType::kModel,
             .load_type = LibGcp::ResourceMgr::LoadType::kExternal,
         }},
        {},
        {LibGcp::StaticObjectSpec{
            .position = {},
            .name     = kMapPath,
        }},
        "first_vertex_shader//first_fragment_shader"
    );
}
