#include <libcgp/main.hpp>

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
                .paths     = {"./models/backpack/backpack.obj"},
                .type      = LibGcp::ResourceMgr::ResourceType::kModel,
                .load_type = LibGcp::ResourceMgr::LoadType::kExternal,
            },
        }
    );
}
