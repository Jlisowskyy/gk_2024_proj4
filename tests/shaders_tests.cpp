#include <gtest/gtest.h>

#include <libcgp/shader.hpp>
#include <open_gl_test.hpp>

#include <utility>

class ShaderTest : public OpenGLTest
{
};

TEST_F(ShaderTest, ShaderClassTest)
{
    const char *const frag_shader =
        R"(#version 460 core
out vec4 FragColor;
in vec3 ourColor;

void main()
{
    FragColor = vec4(0.5, 0.0, 0.0, 1.0);
})";

    const char *const vertex_shader =
        R"(#version 460 core
layout (location = 0) in vec3 aPos;

void main()
{
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
})";

    auto shader = LibGcp::MakeShaderFromCode(vertex_shader, frag_shader);

    /* move shdaer */
    const auto shader2 = std::move(shader);
}
