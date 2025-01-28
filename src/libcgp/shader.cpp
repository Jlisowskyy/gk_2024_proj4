#include <glad/gl.h>

#include <libcgp/shader.hpp>
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
    ENSURE_SUCCESSS_PROGRAM_OPENGL(shader_program);

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

LibGcp::Shader LibGcp::MakeShaderFromCode(const char *vertex_shader_code, const char *fragment_shader_code) noexcept
{
    return {vertex_shader_code, fragment_shader_code};
}

LibGcp::Shader LibGcp::MakeShaderFromName(const char *vertex_shader_name, const char *fragment_shader_name) noexcept
{
    R_ASSERT(StaticShaders::g_KnownVertexShaders.contains(vertex_shader_name));
    R_ASSERT(StaticShaders::g_KnownFragmentShaders.contains(fragment_shader_name));

    return {
        StaticShaders::g_KnownVertexShaders.at(vertex_shader_name),
        StaticShaders::g_KnownFragmentShaders.at(fragment_shader_name)
    };
}

LibGcp::Shader LibGcp::MakeShaderFromFile(const char *vertex_shader_path, const char *fragment_shader_path)
{
    std::string vertex_shader_code;
    std::string fragment_shader_code;

    std::ifstream vertex_shader_file;
    std::ifstream fragment_shader_file;

    vertex_shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fragment_shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {
        std::stringstream vertex_shader_stream;
        std::stringstream fragment_shader_stream;

        vertex_shader_file.open(vertex_shader_path);
        fragment_shader_file.open(fragment_shader_path);

        vertex_shader_stream << vertex_shader_file.rdbuf();
        fragment_shader_stream << fragment_shader_file.rdbuf();

        vertex_shader_file.close();
        fragment_shader_file.close();

        vertex_shader_code   = vertex_shader_stream.str();
        fragment_shader_code = fragment_shader_stream.str();
    } catch (const std::ifstream::failure &e) {
        throw std::runtime_error("Error: Shader file not successfully read: " + std::string(e.what()));
    }

    return {vertex_shader_code.c_str(), fragment_shader_code.c_str()};
}
