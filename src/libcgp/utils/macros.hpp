#ifndef UTILS_MACROS_HPP_
#define UTILS_MACROS_HPP_

// clang-format off
#include <glad/gl.h>
#include <GLFW/glfw3.h> /* (include after glad) */
// clang-format on
#include <cstdlib>
#include <iostream>

#include <libcgp/utils/checks.hpp>

/* Convert a token into a string */
#define STRINGIFY(x) #x
/* Apply STRINGIFY to expand macros before conversion */
#define TOSTRING(x) STRINGIFY(x)

#define R_ASSERT(x)                                                                 \
    if (!(x)) {                                                                               \
        std::cerr << "Error: " #x " at line: " TOSTRING(__LINE__) " at file: " __FILE__ "\n"; \
        glfwTerminate();                                                                      \
        std::abort();                                                                         \
    }

#define ENSURE_SUCCESS_SHADER_OPENGL(shader_id)   CheckShaderErrorsOpenGl(shader_id, __FILE__, __LINE__)
#define ENSURE_SUCESSS_PROGRAM_OPENGL(program_id) CheckProgramErrorsOpenGl(program_id, __FILE__, __LINE__)

#endif  // UTILS_MACROS_HPP_
