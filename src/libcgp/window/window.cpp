#include <libcgp/engine/engine.hpp>
#include <libcgp/window/window.hpp>

#include <libcgp/utils/macros.hpp>

#include <iostream>

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
    glfwWindowHint(GLFW_DEPTH_BITS, 32);

    /* create window */
    GLFWwindow *window = glfwCreateWindow(kWidth, kHeight, "RenderEngine", nullptr, nullptr);
    window_            = window;
    R_ASSERT(window != nullptr);

    glfwMakeContextCurrent(window);

    /* setup window options */
    SwitchMouseLock(true);

    /* setup callbacks */
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetFramebufferSizeCallback(window, FrameBufferSizeCallback);
    glfwSetCursorPosCallback(window, MouseCallback);

    const auto version = gladLoadGL(glfwGetProcAddress);
    R_ASSERT(version);
    TRACE("Loaded OpenGL " << GLAD_VERSION_MAJOR(version) << "." << GLAD_VERSION_MINOR(version));

    // Define the viewport dimensions
    glViewport(0, 0, kWidth, kHeight);

    glEnable(GL_DEPTH_TEST);

    /* disable vsync */
    // glfwSwapInterval(0);

    SyncMousePositionWithWindow_();
}

// ------------------------------
// Callback implementation
// ------------------------------

void LibGcp::Window::SyncMousePositionWithWindow_()
{
    double x_pos;
    double y_pos;

    glfwGetCursorPos(window_, &x_pos, &y_pos);

    mouse_.Reset(x_pos, y_pos);
}

void LibGcp::Window::KeyCallback(
    GLFWwindow *window, const int key, UNUSED int scancode, const int action, UNUSED int mode
)
{
    TRACE("Key pressed " << key << " scancode " << scancode << " mode " << mode);

    if (key != GLFW_DONT_CARE && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        Engine::GetInstance().ButtonPressed(key);
    }
}

void LibGcp::Window::FrameBufferSizeCallback(UNUSED GLFWwindow *window, const int width, const int height)
{
    TRACE("framebuffer_size_callback");

    glViewport(0, 0, width, height);
    GetInstance().SyncMousePositionWithWindow_();

    Engine::GetInstance().GetView().SetWindowAspectRatio(GetInstance().GetAspectRatio());
}

void LibGcp::Window::MouseCallback(UNUSED GLFWwindow *window, const double xpos, const double ypos)
{
    GetInstance().mouse_.Move(xpos, ypos);
}
