#include <libcgp/engine/engine.hpp>
#include <libcgp/window/window.hpp>

#include <libcgp/utils/macros.hpp>

#include <iostream>

// ------------------------------
// Window Callbacks
// ------------------------------

static void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode);

static void FrameBufferSizeCallback(GLFWwindow *window, int width, int height);

static void WindowSizeCallback(GLFWwindow *window, int width, int height);

static void MouseCallback(GLFWwindow *window, double xpos, double ypos);

// ------------------------------
// Constants
// ------------------------------

static constexpr GLuint kWidth  = 800;
static constexpr GLuint kHeight = 600;

// ------------------------------
// Class implementation
// ------------------------------

LibGcp::Window::~Window()
{
    // Terminates GLFW, clearing any resources allocated by GLFW.
    glfwTerminate();
}

void LibGcp::Window::Init()
{
    glfwInit();

    /* setup initial config */
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    /* create window */
    GLFWwindow *window = glfwCreateWindow(kWidth, kHeight, "RenderEngine", nullptr, nullptr);
    R_ASSERT(window != nullptr);

    glfwMakeContextCurrent(window);

    /* setup window options */
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    /* setup callbacks */
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetFramebufferSizeCallback(window, FrameBufferSizeCallback);
    glfwSetWindowSizeCallback(window, WindowSizeCallback);
    glfwSetCursorPosCallback(window, MouseCallback);

    const auto version = gladLoadGL(glfwGetProcAddress);
    R_ASSERT(version);
    TRACE("Loaded OpenGL " << GLAD_VERSION_MAJOR(version) << "." << GLAD_VERSION_MINOR(version));

    // Define the viewport dimensions
    glViewport(0, 0, kWidth, kHeight);

    glEnable(GL_DEPTH_TEST);
    window_ = window;
}

// ------------------------------
// Callback implementation
// ------------------------------

void KeyCallback(
    GLFWwindow *window, const int key, [[maybe_unused]] int scancode, const int action, [[maybe_unused]] int mode
)
{
    TRACE("Key pressed " << key << " scancode " << scancode << " mode " << mode);

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
        return;
    }

    if (key != GLFW_DONT_CARE) {
        LibGcp::Engine::GetInstance().ButtonPressed(key);
    }
}

void FrameBufferSizeCallback([[maybe_unused]] GLFWwindow *window, const int width, const int height)
{
    TRACE("framebuffer_size_callback");

    glViewport(0, 0, width, height);
}

void WindowSizeCallback([[maybe_unused]] GLFWwindow *window, const int width, const int height)
{
    TRACE("window_size_callback");

    glViewport(0, 0, width, height);
}
void MouseCallback(GLFWwindow *window, double xpos, double ypos) {}
