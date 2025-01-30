#include <libcgp/main.hpp>

#include <libcgp/mgr/object_mgr.hpp>
#include <libcgp/mgr/resource_mgr.hpp>
#include <libcgp/mgr/settings_mgr.hpp>

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
        },
        {},
        [] {
            glm::vec3 positions[] = {glm::vec3(0.0f, 0.0f, 0.0f),    glm::vec3(2.0f, 5.0f, -15.0f),
                                     glm::vec3(-1.5f, -2.2f, -2.5f), glm::vec3(-3.8f, -2.0f, -12.3f),
                                     glm::vec3(2.4f, -0.4f, -3.5f),  glm::vec3(-1.7f, 3.0f, -7.5f),
                                     glm::vec3(1.3f, -2.0f, -2.5f),  glm::vec3(1.5f, 2.0f, -2.5f),
                                     glm::vec3(1.5f, 0.2f, -1.5f),   glm::vec3(-1.3f, 1.0f, -1.5f)};

            LibGcp::ObjectMgr::static_objects_t static_objects{};

            for (const auto &pos : positions) {
                static_objects.push_back({pos, "./models/backpack/backpack.obj"});
            }

            return static_objects;
        }()
    );
}
