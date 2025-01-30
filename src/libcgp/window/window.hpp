#ifndef WINDOW_WINDOW_HPP_
#define WINDOW_WINDOW_HPP_

#include <CxxUtils/singleton.hpp>
#include <libcgp/defines.hpp>

#include <cassert>

struct GLFWwindow;

LIBGCP_DECL_START_

class Window : public CxxUtils::Singleton<Window>
{
    protected:
    // ------------------------------
    // Class creation
    // ------------------------------

    Window() = default;

    public:
    ~Window();

    FAST_CALL static Window &InitInstance()
    {
        assert(!IsInited());

        Singleton::InitInstance(new Window());
        return GetInstance();
    }

    // ------------------------------
    // Class interaction
    // ------------------------------

    void Init();

    void RunLoop();

    // ----------------------------------
    // Class implementation methods
    // ----------------------------------

    protected:
    // ------------------------------
    // Class fields
    // ------------------------------

    GLFWwindow *window_{};
};

LIBGCP_DECL_END_

#endif  // WINDOW_WINDOW_HPP_
