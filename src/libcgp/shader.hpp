#ifndef LIBGCP_SHADER_HPP_
#define LIBGCP_SHADER_HPP_

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <libcgp/defines.hpp>

#include <cassert>

// ------------------------------
// Helper macros
// ------------------------------

#define GENERATE_UNIFORM_SETTER_SAFE_(TypeName, UniformFunc)                \
    void Set##TypeName(const char *name, const TypeName &value) const       \
    {                                                                       \
        const GLint location = glGetUniformLocation(shader_program_, name); \
        assert(location != -1);                                             \
        UniformFunc(location, value);                                       \
    }

#define GENERATE_VECTOR_UNIFORM_SETTER_SAFE_(funcName, TypeName, UniformFunc)             \
    void Set##funcName(const char *name, const TypeName &values, GLsizei count = 1) const \
    {                                                                                     \
        const GLint location = glGetUniformLocation(shader_program_, name);               \
        assert(location != -1);                                                           \
        UniformFunc(location, count, GL_FALSE, glm::value_ptr(values));                   \
    }

#define GENERATE_UNIFORM_SETTER_UNSAFE_(TypeName, UniformFunc)                \
    void Set##TypeName##Unsafe(const char *name, const TypeName &value) const \
    {                                                                         \
        UniformFunc(glGetUniformLocation(shader_program_, name), value);      \
    }

#define GENERATE_VECTOR_UNIFORM_SETTER_UNSAFE_(funcName, TypeName, UniformFunc)                            \
    void Set##funcName##Unsafe(const char *name, const TypeName &values, GLsizei count = 1) const          \
    {                                                                                                      \
        UniformFunc(glGetUniformLocation(shader_program_, name), count, GL_FALSE, glm::value_ptr(values)); \
    }

#define GENERATE_UNIFORM_SETTER_(TypeName, UniformFunc)  \
    GENERATE_UNIFORM_SETTER_SAFE_(TypeName, UniformFunc) \
    GENERATE_UNIFORM_SETTER_UNSAFE_(TypeName, UniformFunc)

#define GENERATE_VECTOR_UNIFORM_SETTER_(funcName, TypeName, UniformFunc)  \
    GENERATE_VECTOR_UNIFORM_SETTER_SAFE_(funcName, TypeName, UniformFunc) \
    GENERATE_VECTOR_UNIFORM_SETTER_UNSAFE_(funcName, TypeName, UniformFunc)

LIBGCP_DECL_START_
// ------------------------------
// Shader class
// ------------------------------

class Shader
{
    // ------------------------------
    // Inner types
    // ------------------------------

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

    /* simple uniform setters */
    GENERATE_UNIFORM_SETTER_(GLint, glUniform1i)
    GENERATE_UNIFORM_SETTER_(GLfloat, glUniform1f)
    GENERATE_UNIFORM_SETTER_(GLuint, glUniform1ui)

    /* matrix uniform setters */
    GENERATE_VECTOR_UNIFORM_SETTER_(Mat2, glm::mat2, glUniformMatrix2fv)
    GENERATE_VECTOR_UNIFORM_SETTER_(Mat3, glm::mat3, glUniformMatrix3fv)
    GENERATE_VECTOR_UNIFORM_SETTER_(Mat4, glm::mat4, glUniformMatrix4fv)

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
