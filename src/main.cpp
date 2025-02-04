#include <libcgp/main.hpp>

#include <libcgp/intf.hpp>
#include <libcgp/mgr/object_mgr.hpp>
#include <libcgp/mgr/resource_mgr.hpp>
#include <libcgp/mgr/settings_mgr.hpp>

static constexpr const char *kMapPath1 = "./models/town_map/source/town4new.glb";
static constexpr const char *kMapPath  = "./models/csgo.glb";

#ifdef NDEBUG

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
         LibGcp::ResourceMgr::ResourceSpec{
             .paths     = {"./models/backpack/backpack.obj"},
             .type      = LibGcp::ResourceMgr::ResourceType::kModel,
             .load_type = LibGcp::ResourceMgr::LoadType::kExternal,
         },
         LibGcp::ResourceMgr::ResourceSpec{
             .paths     = {kMapPath},
             .type      = LibGcp::ResourceMgr::ResourceType::kModel,
             .load_type = LibGcp::ResourceMgr::LoadType::kExternal,
         }},
        {},
        {
            LibGcp::StaticObjectSpec{
                .position = {},
                .name     = kMapPath,
            },
            // LibGcp::StaticObjectSpec{
            //     .position = {},
            //     .name = "./models/backpack/backpack.obj",
            // }
        },
        "first_vertex_shader//first_fragment_shader"
    );
}

#else

int main()
{
    return LibGcp::RenderEngineMain(
        {
    },
        {
            LibGcp::ResourceMgr::ResourceSpec{
                .paths     = {"first_vertex_shader", "first_fragment_shader"},
                .type      = LibGcp::ResourceMgr::ResourceType::kShader,
                .load_type = LibGcp::ResourceMgr::LoadType::kMemory,
            },
            LibGcp::ResourceMgr::ResourceSpec{
                .paths     = {"contours", "contours"},
                .type      = LibGcp::ResourceMgr::ResourceType::kShader,
                .load_type = LibGcp::ResourceMgr::LoadType::kMemory,
            },
            LibGcp::ResourceMgr::ResourceSpec{
                .paths        = {"./models/backpack/backpack.obj"},
                .type         = LibGcp::ResourceMgr::ResourceType::kModel,
                .load_type    = LibGcp::ResourceMgr::LoadType::kExternal,
                .flip_texture = 1,
            },
            LibGcp::ResourceMgr::ResourceSpec{
                .paths        = {"./models/bulb.glb"},
                .type         = LibGcp::ResourceMgr::ResourceType::kModel,
                .load_type    = LibGcp::ResourceMgr::LoadType::kExternal,
                .flip_texture = 0,
            },
        },
        {},
        {LibGcp::StaticObjectSpec{
             .position = {},
             .name     = "./models/backpack/backpack.obj",
         },
         LibGcp::StaticObjectSpec{
             .position =
                 {
                     .position = {5.0f, 5.0f, 5.0f},
                     .rotation = {1.6f, 0.0f, 0.0f},
                 },
             .name = "./models/bulb.glb",
         }},
        "first_vertex_shader//first_fragment_shader"
    );
}

#endif
