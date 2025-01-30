#include <libcgp/mgr/object_mgr.hpp>
#include <libcgp/mgr/resource_mgr.hpp>
#include <libcgp/primitives/shader.hpp>
#include <libcgp/primitives/static_object.hpp>

#include <vector>

LibGcp::ObjectMgr::~ObjectMgr() { TRACE("ObjectMgr::~ObjectMgr()"); }

void LibGcp::ObjectMgr::Init(
    const static_objects_t &static_objects, [[maybe_unused]] const dynamic_objects_t &dynamic_objects
)
{
    static_objects_.reserve(kDefaultStorageSize);

    for (const auto &spec : static_objects) {
        CreateStaticObject_(spec);
    }
}

void LibGcp::ObjectMgr::DrawStaticObjects(Shader &shader)
{
    for (const auto &object : static_objects_) {
        object.Draw(shader);
    }
}

void LibGcp::ObjectMgr::CreateStaticObject_(const StaticObjectSpec &spec)
{
    static_objects_.emplace_back(
        spec.position, ResourceMgr::GetInstance().GetModel(spec.name, ResourceMgr::LoadType::kExternal)
    );
}

void LibGcp::ObjectMgr::CreateDynamicObject_([[maybe_unused]] const DynamicObjectSpec &spec) {}
