#ifndef UTILS_CHECKS_HPP_
#define UTILS_CHECKS_HPP_

#include <glad/gl.h>

void CheckShaderErrorsOpenGl(GLuint shader_id, const char *file, int line);
void CheckProgramErrorsOpenGl(GLuint program_id, const char *file, int line);

#endif  // UTILS_CHECKS_HPP_
