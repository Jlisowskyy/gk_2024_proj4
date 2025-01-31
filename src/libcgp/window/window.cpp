#include <libcgp/engine/engine.hpp>
#include <libcgp/window/window.hpp>

#include <libcgp/utils/macros.hpp>

#include <iostream>

// Function prototypes
static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);

static void framebuffer_size_callback(GLFWwindow *window, int width, int height);

static void window_size_callback(GLFWwindow *window, int width, int height);

// Window dimensions
static constexpr GLuint kWidth  = 800;
static constexpr GLuint kHeight = 600;

LibGcp::Window::~Window()
{
    // Terminates GLFW, clearing any resources allocated by GLFW.
    glfwTerminate();
}

void LibGcp::Window::Init()
{
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow *window = glfwCreateWindow(kWidth, kHeight, "RenderEngine", nullptr, nullptr);
    R_ASSERT(window != nullptr);

    glfwMakeContextCurrent(window);

    // Set the required callback functions
    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetWindowSizeCallback(window, window_size_callback);

    // Load OpenGL functions, gladLoadGL returns the loaded version, 0 on error.
    const auto version = gladLoadGL(glfwGetProcAddress);
    R_ASSERT(version);
    TRACE("Loaded OpenGL " << GLAD_VERSION_MAJOR(version) << "." << GLAD_VERSION_MINOR(version));

    // Define the viewport dimensions
    glViewport(0, 0, kWidth, kHeight);

    glEnable(GL_DEPTH_TEST);
    window_ = window;
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(
    GLFWwindow *window, const int key, [[maybe_unused]] int scancode, const int action, [[maybe_unused]] int mode
)
{
    TRACE("Key pressed");

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
        return;
    }

    LibGcp::Engine::GetInstance().ButtonPressed(key);
}

void framebuffer_size_callback([[maybe_unused]] GLFWwindow *window, const int width, const int height)
{
    TRACE("framebuffer_size_callback");

    glViewport(0, 0, width, height);
}

void window_size_callback([[maybe_unused]] GLFWwindow *window, const int width, const int height)
{
    TRACE("window_size_callback");

    glViewport(0, 0, width, height);
}
