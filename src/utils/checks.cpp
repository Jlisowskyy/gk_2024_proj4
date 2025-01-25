#include <utils/checks.hpp>

// clang-format off
#include <glad/gl.h>
#include <GLFW/glfw3.h> /* (include after glad) */
// clang-format on

#include <array>
#include <cstdlib>
#include <iostream>

static constexpr size_t kLogBufferSize = 512;

void CheckShaderErrorsOpenGl(const GLuint shader_id, const char *file, const int line)
{
    int success{};

    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);
    if (success != 0) {
        return;
    }

    std::array<char, kLogBufferSize> info_log{};

    glGetShaderInfoLog(shader_id, kLogBufferSize, nullptr, info_log.data());

    std::cerr << "[ERROR] " << file << ' ' << line << ' ' << info_log.data() << '\n';
    glfwTerminate();
    std::abort();
}

void CheckProgramErrorsOpenGl(GLuint program_id, const char *file, int line)
{
    int success{};

    glGetProgramiv(program_id, GL_LINK_STATUS, &success);
    if (success != 0) {
        return;
    }

    std::array<char, kLogBufferSize> info_log{};

    glGetProgramInfoLog(program_id, kLogBufferSize, nullptr, info_log.data());
    std::cerr << "[ERROR] " << file << ' ' << line << ' ' << info_log.data() << '\n';
    glfwTerminate();
    std::abort();
}
