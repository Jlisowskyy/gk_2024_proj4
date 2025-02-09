#ifndef ENGINE_ENGINE_HPP_
#define ENGINE_ENGINE_HPP_

#include <libcgp/defines.hpp>
#include <libcgp/engine/global_light.hpp>
#include <libcgp/engine/view.hpp>
#include <libcgp/engine/word_time.hpp>
#include <libcgp/primitives/shader.hpp>

#include <CxxUtils/static_singleton.hpp>

#include <array>
#include <cassert>
#include <cstdint>
#include <memory>

// clang-format off
#include <glad/gl.h>
#include <GLFW/glfw3.h> /* (include after glad) */
// clang-format on

LIBGCP_DECL_START_
class EngineBase final : public CxxUtils::StaticSingletonHelper
{
    // ------------------------------
    // Class internals
    // ------------------------------

    // ------------------------------
    // Object creation
    // ------------------------------

    public:
    EngineBase() = default;
    ~EngineBase() override;

    // ------------------------------
    // Class interaction
    // ------------------------------

    void Init(const Scene &scene) noexcept;

    FAST_CALL View &GetView() noexcept { return view_; }

    void ProcessProgress(uint64_t delta);

    FAST_CALL void ButtonPressed(const int key) { ++keys_[key]; }

    NDSCRD FAST_CALL std::shared_ptr<Shader> GetDefaultShader() const noexcept { return default_shader_; }

    void ReloadScene(const Scene &scene);

    /* camera */
    NDSCRD FAST_CALL const CameraInfo &GetFreeCamera() const noexcept { return free_camera_; }

    FAST_CALL void MoveFreeCameraTo(const glm::vec3 &position) { free_camera_.position = position; }

    // ---------------------------------
    // Class implementation methods
    // ---------------------------------

    protected:
    void ProcessInput_(uint64_t delta);

    void ProcessUserMovement_(uint64_t delta);

    void ProcessFreeCameraMovement_(uint64_t delta);

    void ProcessDynamicObjects_(uint64_t delta);

    static void OnCameraTypeChanged_(uint64_t new_value);

    static void OnDefaultShaderChanged_(uint64_t new_value);

    static std::shared_ptr<Shader> FindShaderWithId(uint64_t id);

    // ------------------------------
    // Class fields
    // ------------------------------

    WordTime word_time_{};

    // TODO: temp object
    CameraInfo flowing_camera_{};
    double flow_count_{};

    CameraInfo free_camera_{};
    const CameraInfo static_camera_{
        .position = glm::vec3{0.0f, 0.0f, 3.0f},
        .front    = glm::vec3(0.0f, 0.0f, -1.0f),
        .up       = glm::vec3(0.0f, 1.0f, 0.0f),
    };

    std::array<int, GLFW_KEY_LAST> keys_{};
    View view_{};

    std::shared_ptr<Shader> default_shader_{};
};

using Engine = CxxUtils::StaticSingleton<EngineBase>;

LIBGCP_DECL_END_

#endif  // ENGINE_ENGINE_HPP_
