#include <glad/gl.h>

#include <libcgp/primitives/shader.hpp>
#include <libcgp/utils/macros.hpp>
#include <shaders/static_header.hpp>

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

LibGcp::Shader::Shader(const char *vertex_shader_code, const char *fragment_shader_code) noexcept
{
    // Load VertexShader
    const auto vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_code, nullptr);
    glCompileShader(vertex_shader);

    // Check for shader compile errors
    ENSURE_SUCCESS_SHADER_OPENGL(vertex_shader);

    // Load FragmentShader
    const auto fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_code, nullptr);
    glCompileShader(fragment_shader);

    // Check for shader compile errors
    ENSURE_SUCCESS_SHADER_OPENGL(fragment_shader);

    // create shader program
    const auto shader_program = glCreateProgram();

    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);

    // Check for linking errors
    ENSURE_SUCCESS_PROGRAM_OPENGL(shader_program);

    // delete shaders
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    // store shader program
    shader_program_ = shader_program;
}

LibGcp::Shader::~Shader() noexcept
{
    if (shader_program_ != 0) {
        glDeleteProgram(shader_program_);
        shader_program_ = 0;
    }
}
