#ifndef OPEN_GL_TEST_HPP_
#define OPEN_GL_TEST_HPP_

#include <gtest/gtest.h>

// clang-format off
#include <glad/gl.h>
#include <GLFW/glfw3.h>
// clang-format on

class OpenGLTest : public ::testing::Test
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

#endif  // OPEN_GL_TEST_HPP_
