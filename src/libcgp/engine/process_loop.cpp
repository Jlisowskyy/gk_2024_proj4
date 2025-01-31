#include <libcgp/engine/process_loop.hpp>
#include <libcgp/mgr/object_mgr.hpp>
#include <libcgp/window/window.hpp>

// clang-format off
#include <glad/gl.h>
#include <GLFW/glfw3.h> /* (include after glad) */
// clang-format on

#include <chrono>
#include <string>

void LibGcp::ProcessLoopApp(const std::string& default_shader_name)
{
    auto shader = ResourceMgr::GetInstance().GetShader(default_shader_name, ResourceMgr::LoadType::kMemory);
    shader->Activate();

    auto last_frame = std::chrono::steady_clock::now();
    Window::GetInstance().RunLoop([&] {
        // Render
        // Clear the color buffer
        glClearColor(0.2F, 0.3F, 0.3F, 1.0F);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //  Render objects
        ObjectMgr::GetInstance().DrawStaticObjects(*shader);

        // Process progress
        const auto new_frame = std::chrono::steady_clock::now();
        const auto delta_time_ms =
            std::chrono::duration_cast<std::chrono::microseconds>(new_frame - last_frame).count();
        last_frame = new_frame;

        ObjectMgr::GetInstance().ProcessProgress(delta_time_ms);
    });
}
