#include <libcgp/engine/light_mgr.hpp>

#include <libcgp/engine/lights.hpp>
#include <libcgp/engine/view.hpp>
#include <libcgp/intf.hpp>
#include <libcgp/mgr/object_mgr.hpp>
#include <libcgp/mgr/resource_mgr.hpp>
#include <libcgp/primitives/shader.hpp>
#include <libcgp/utils/macros.hpp>

#include <cassert>

// ------------------------------
// Statics
// ------------------------------

LIBGCP_DECL_START_
static constexpr size_t kPointLightIdx = 0;
static constexpr size_t kSpotLightIdx  = 1;

template <class T>
void LightGetFunc(Shader &shader, uint64_t *counter, const T &light, const glm::mat4 &mm, const glm::mat4 &rm)
{
    NOT_IMPLEMENTED
}

template <>
void LightGetFunc<>(Shader &shader, uint64_t *counter, const PointLight &light, const glm::mat4 &mm, const glm::mat4 &)
{
    const auto word_pos = glm::vec3(mm * glm::vec4(light.light_info.position, 1.0));

    const std::string prefix = "lightning.point_lights[" + std::to_string(counter[kPointLightIdx]++) + "].";

    shader.SetVec3Unsafe((prefix + "info.position").c_str(), word_pos);

    shader.SetVec3Unsafe((prefix + "info.ambient").c_str(), light.light_info.ambient);

    shader.SetVec3Unsafe((prefix + "info.diffuse").c_str(), light.light_info.diffuse);

    shader.SetVec3Unsafe((prefix + "info.specular").c_str(), light.light_info.specular);

    shader.SetGLfloatUnsafe((prefix + "info.intensity").c_str(), light.light_info.intensity);

    shader.SetGLfloatUnsafe((prefix + "constant").c_str(), light.point_light.constant);

    shader.SetGLfloatUnsafe((prefix + "linear").c_str(), light.point_light.linear);

    shader.SetGLfloatUnsafe((prefix + "quadratic").c_str(), light.point_light.quadratic);
}

template <>
void LightGetFunc<>(Shader &shader, uint64_t *counter, const SpotLight &light, const glm::mat4 &mm, const glm::mat4 &rm)
{
    const auto word_pos = glm::vec3(mm * glm::vec4(light.light_info.position, 1.0));
    const auto word_dir = glm::vec3(rm * glm::vec4(light.spot_light.direction, 0.0));

    const std::string prefix = "lightning.spot_lights[" + std::to_string(counter[kSpotLightIdx]++) + "].";

    shader.SetVec3Unsafe((prefix + "info.position").c_str(), word_pos);

    shader.SetVec3Unsafe((prefix + "info.ambient").c_str(), light.light_info.ambient);

    shader.SetVec3Unsafe((prefix + "info.diffuse").c_str(), light.light_info.diffuse);

    shader.SetVec3Unsafe((prefix + "info.specular").c_str(), light.light_info.specular);

    shader.SetGLfloatUnsafe((prefix + "info.intensity").c_str(), light.light_info.intensity);

    shader.SetVec3Unsafe((prefix + "direction").c_str(), word_dir);
    shader.SetGLfloatUnsafe((prefix + "constant").c_str(), light.spot_light.constant);
    shader.SetGLfloatUnsafe((prefix + "linear").c_str(), light.spot_light.linear);
    shader.SetGLfloatUnsafe((prefix + "quadratic").c_str(), light.spot_light.quadratic);
    shader.SetGLfloatUnsafe((prefix + "cut_off").c_str(), light.spot_light.cut_off);
    shader.SetGLfloatUnsafe((prefix + "outer_cut_off").c_str(), light.spot_light.outer_cut_off);
}

LIBGCP_DECL_END_

// ------------------------------
// Implementations
// ------------------------------

void LibGcp::LightMgr::LoadLightsFromScene(const Scene &scene)
{
    /* Load point lights */
    for (const auto &point_light_spec : scene.point_lights) {
        auto model = ResourceMgr::GetInstance().GetModel(point_light_spec.model_name, LoadType::kExternal);
        R_ASSERT(model != nullptr && "Model not found for light object!");

        model->GetLights().emplace_back<PointLight>(point_light_spec);
        ++point_light_count_;
    }

    /* Load spotlights */
    for (const auto &spot_light_spec : scene.spot_lights) {
        auto model = ResourceMgr::GetInstance().GetModel(spot_light_spec.model_name, LoadType::kExternal);
        R_ASSERT(model != nullptr && "Model not found for light object!");

        model->GetLights().emplace_back<SpotLight>(spot_light_spec);
        ++spot_light_count_;
    }

    R_ASSERT(point_light_count_ <= kMaxTypeLightObjects && "Too many point lights");
    R_ASSERT(spot_light_count_ <= kMaxTypeLightObjects && "Too many spot lights");
    R_ASSERT(point_light_count_ + spot_light_count_ <= kMaxLightObjects && "Too many lights");
}

void LibGcp::LightMgr::PrepareLights(Shader &shader) const
{
    shader.SetGLuintUnsafe("lightning.num_point_lights", point_light_count_);
    shader.SetGLuintUnsafe("lightning.num_spot_lights", spot_light_count_);

    uint64_t counters[2]{};
    for (const auto &obj : ObjectMgr::GetInstance().GetStaticObjects()) {
        const auto model = obj.GetModel();

        if (model->GetLights().IsEmpty()) {
            continue;
        }

        const auto model_matrix = View::PrepareModelMatrices(obj.GetPosition());
        const auto rot_matrix   = View::PrepareRotMatrix(obj.GetPosition());
        model->GetLights().Foreach([&]<class T>(const T &light) {
            LightGetFunc(shader, counters, light, model_matrix, rot_matrix);
        });
    }
}
