#include <gtest/gtest.h>

#include <libcgp/shader.hpp>
#include <shaders/static_header.hpp>

// clang-format off
#include <glad/gl.h>
#include <GLFW/glfw3.h>
// clang-format on

class ShaderTest : public ::testing::Test
{
    protected:
    GLFWwindow* offscreenContext{};

    void SetUp() override
    {
        ASSERT_TRUE(glfwInit()) << "Failed to initialize GLFW";

        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);  // Create a hidden window
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        offscreenContext = glfwCreateWindow(1, 1, "", nullptr, nullptr);
        ASSERT_TRUE(offscreenContext) << "Failed to create offscreen GLFW window";

        glfwMakeContextCurrent(offscreenContext);

        const auto version = gladLoadGL(glfwGetProcAddress);
        ASSERT_TRUE(version) << "Failed to initialize GLAD";
    }

    void TearDown() override
    {
        glfwDestroyWindow(offscreenContext);
        glfwTerminate();
    }
};

TEST_F(ShaderTest, ShaderClassTest)
{
    const char* const frag_shader =
        R"(#version 460 core
out vec4 FragColor;
in vec3 ourColor;

void main()
{
    FragColor = vec4(0.5, 0.0, 0.0, 1.0);
})";

    const char* const vertex_shader =
        R"(#version 460 core
layout (location = 0) in vec3 aPos;

void main()
{
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
})";

    const auto shader = LibGcp::MakeShaderFromCode(vertex_shader, frag_shader);
}
