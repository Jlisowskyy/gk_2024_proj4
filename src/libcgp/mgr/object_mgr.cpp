#include <libcgp/engine/engine.hpp>
#include <libcgp/mgr/object_mgr.hpp>
#include <libcgp/mgr/resource_mgr.hpp>
#include <libcgp/primitives/shader.hpp>
#include <libcgp/primitives/static_object.hpp>

#include <vector>

LibGcp::ObjectMgrBase::~ObjectMgrBase() { TRACE("ObjectMgrBase::~ObjectMgrBase()"); }

void LibGcp::ObjectMgrBase::Init(
    const static_objects_t &static_objects, [[maybe_unused]] const dynamic_objects_t &dynamic_objects
)
{
    static_objects_.reserve(kDefaultStorageSize);

    for (const auto &spec : static_objects) {
        CreateStaticObject_(spec);
    }
}

void LibGcp::ObjectMgrBase::DrawStaticObjects(Shader &shader)
{
    Engine::GetInstance().GetView().PrepareViewMatrices(shader);

    for (const auto &object : static_objects_) {
        Engine::GetInstance().GetView().PrepareModelMatrices(shader, object.GetPosition());
        object.Draw(shader);
    }
}

void LibGcp::ObjectMgrBase::ProcessProgress([[maybe_unused]] long delta_time_micros) {}

void LibGcp::ObjectMgrBase::RemoveStaticObject(const uint64_t ident)
{
    std::lock_guard lock(static_objects_.GetMutex());

    const auto obj_it = std::ranges::find_if(static_objects_, [ident](const StaticObject &obj) {
        return obj.GetId() == ident;
    });
    assert(obj_it != static_objects_.end());

    static_objects_.GetListeners().NotifyListeners<CxxUtils::ContainerEvents::kRemove>(*obj_it);
    static_objects_.erase(obj_it);
}

void LibGcp::ObjectMgrBase::CreateStaticObject_(const StaticObjectSpec &spec)
{
    std::lock_guard lock(static_objects_.GetMutex());
    auto obj = static_objects_.emplace_back(
        spec.position, ResourceMgr::GetInstance().GetModel(spec.name, LoadType::kExternal)
    );
    static_objects_.GetListeners().NotifyListeners<CxxUtils::ContainerEvents::kAdd>(obj);
}

void LibGcp::ObjectMgrBase::CreateDynamicObject_([[maybe_unused]] const DynamicObjectSpec &spec) {}
