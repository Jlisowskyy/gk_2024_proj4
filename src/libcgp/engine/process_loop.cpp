#include <libcgp/engine/process_loop.hpp>
#include <libcgp/window/window.hpp>

// clang-format off
#include <glad/gl.h>
#include <GLFW/glfw3.h> /* (include after glad) */
// clang-format on

void LibGcp::ProcessLoopApp()
{
    Window::GetInstance().RunLoop([&]() {
        // Render
        // Clear the color buffer
        glClearColor(0.2F, 0.3F, 0.3F, 1.0F);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //  Render objects

        // Process progress
    });
}
