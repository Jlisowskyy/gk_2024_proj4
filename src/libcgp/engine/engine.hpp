#ifndef ENGINE_ENGINE_HPP_
#define ENGINE_ENGINE_HPP_

#include <libcgp/defines.hpp>

#include <CxxUtils/singleton.hpp>
#include <libcgp/settings.hpp>

#include <libcgp/engine/view.hpp>

#include <cassert>
#include <cstdint>

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

    // ---------------------------------
    // Class implementation methods
    // ---------------------------------

    protected:
    static void OnCameraTypeChanged(uint64_t new_value);

    // ------------------------------
    // Class fields
    // ------------------------------

    View view_{};
};

LIBGCP_DECL_END_

#endif  // ENGINE_ENGINE_HPP_
