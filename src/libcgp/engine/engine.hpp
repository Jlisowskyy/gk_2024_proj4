#ifndef ENGINE_ENGINE_HPP_
#define ENGINE_ENGINE_HPP_

#include <libcgp/defines.hpp>

#include <CxxUtils/singleton.hpp>
#include <libcgp/settings.hpp>

#include <libcgp/engine/view.hpp>

#include <array>
#include <cassert>
#include <cstdint>

// clang-format off
#include <glad/gl.h>
#include <GLFW/glfw3.h> /* (include after glad) */
// clang-format on

LIBGCP_DECL_START_
class Engine final : public CxxUtils::Singleton<Engine>
{
    // ------------------------------
    // Class internals
    // ------------------------------

    // ------------------------------
    // Object creation
    // ------------------------------

    protected:
    Engine() = default;

    public:
    ~Engine() override;

    FAST_CALL static Engine &InitInstance() noexcept
    {
        assert(!IsInited());

        Singleton::InitInstance(new Engine());
        return GetInstance();
    }

    // ------------------------------
    // Class interaction
    // ------------------------------

    void Init() noexcept;

    FAST_CALL View &GetView() noexcept { return view_; }

    void ProcessProgress(long delta);

    FAST_CALL void ButtonPressed(const int key) { ++keys_[key]; }

    // ---------------------------------
    // Class implementation methods
    // ---------------------------------

    protected:
    void ProcessInput_(long delta);
    void ProcessUserMovement_(long delta);
    void ProcessFreeCameraMovement_(long delta);
    void ProcessDynamicObjects_(long delta);

    static void OnCameraTypeChanged_(uint64_t new_value);

    // ------------------------------
    // Class fields
    // ------------------------------

    // TODO: temp object
    glm::vec3 flow_position_{};
    glm::vec3 flow_direction_{};

    glm::vec3 free_camera_position_{};
    glm::vec3 free_camera_front_{};

    std::array<int, GLFW_KEY_LAST> keys_{};
    View view_{};
};

LIBGCP_DECL_END_

#endif  // ENGINE_ENGINE_HPP_
