#ifndef UTILS_MACROS_HPP_
#define UTILS_MACROS_HPP_

// clang-format off
#include <glad/gl.h>
#include <GLFW/glfw3.h> /* (include after glad) */
// clang-format on
#include <cstdlib>

/* Convert a token into a string */
#define STRINGIFY(x) #x
/* Apply STRINGIFY to expand macros before conversion */
#define TOSTRING(x) STRINGIFY(x)

#define VERIFY_OUTPUT_GLFW(x)                                                                 \
    if (!(x)) {                                                                               \
        std::cerr << "Error: " #x " at line: " TOSTRING(__LINE__) " at file: " __FILE__ "\n"; \
        ;                                                                                     \
        glfwTerminate();                                                                      \
        std::exit(EXIT_FAILURE);                                                              \
    }

#endif  // UTILS_MACROS_HPP_
