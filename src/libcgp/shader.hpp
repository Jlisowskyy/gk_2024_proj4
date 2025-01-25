#ifndef LIBGCP_SHADER_HPP_
#define LIBGCP_SHADER_HPP_

#include <glad/gl.h>

#include <libcgp/defines.hpp>

LIBGCP_DECL_START_
// ------------------------------
// Shader class
// ------------------------------

class Shader
{
    public:
    // ------------------------------
    // Object creation
    // ------------------------------

    Shader(const char *vertex_shader_code, const char *fragment_shader_code) noexcept;

    ~Shader() noexcept;

    /* prohibit copy constructor and assignment operator */
    Shader(const Shader &) = delete;

    Shader &operator=(const Shader &) = delete;

    /* allow move operations as ownership is unique */
    Shader(Shader &&) noexcept = default;

    Shader &operator=(Shader &&) noexcept = default;

    // ------------------------------
    // Class interaction
    // ------------------------------

    WRAP_CALL void Activate() const noexcept { glUseProgram(shader_program_); }

    NDSCRD WRAP_CALL GLuint GetProgram() const noexcept { return shader_program_; }

    // ------------------------------
    // Class fields
    // ------------------------------

    protected:
    GLuint shader_program_{};
};

// ------------------------------
// Factories
// ------------------------------

Shader MakeShaderFromCode(const char *vertex_shader_code, const char *fragment_shader_code) noexcept;
Shader MakeShaderFromName(const char *vertex_shader_name, const char *fragment_shader_name) noexcept;
Shader MakeShaderFromFile(const char *vertex_shader_file, const char *fragment_shader_file);

LIBGCP_DECL_END_

#endif  // LIBGCP_SHADER_HPP_
