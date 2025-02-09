#include <libcgp/engine/engine.hpp>
#include <libcgp/engine/process_loop.hpp>
#include <libcgp/mgr/object_mgr.hpp>
#include <libcgp/window/window.hpp>

// clang-format off
#include <glad/gl.h>
#include <GLFW/glfw3.h> /* (include after glad) */
// clang-format on

#include <chrono>
#include <string>

void LibGcp::ProcessLoopApp()
{
    Window::GetInstance().InitDebug();

    /* At this point all events should be connected */
    auto last_frame = std::chrono::steady_clock::now();
    Window::GetInstance().RunLoop([&] {
        // Render
        // Clear the color buffer
        glClearColor(0.0F, 0.0F, 0.0F, 1.0F);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //  Render objects
        const auto shader = Engine::GetInstance().GetDefaultShader();
        shader->Activate();
        ObjectMgr::GetInstance().DrawStaticObjects(*shader);

        // Process progress
        const auto new_frame = std::chrono::steady_clock::now();
        const uint64_t delta_time_ms =
            std::chrono::duration_cast<std::chrono::microseconds>(new_frame - last_frame).count();
        last_frame = new_frame;

        TRACE("Curr fps: " << 1000000.0F / delta_time_ms);
        Engine::GetInstance().ProcessProgress(delta_time_ms);
    });

    Window::GetInstance().DestroyDebug();
}
