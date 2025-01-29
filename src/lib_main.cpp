#include <array>
#include <cmath>
#include <iostream>

// clang-format off
#include <glad/gl.h>
#include <GLFW/glfw3.h> /* (include after glad) */
// clang-format on

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <libcgp/mesh.hpp>
#include <libcgp/model.hpp>
#include <libcgp/shader.hpp>
#include <libcgp/texture.hpp>
#include <libcgp/utils/macros.hpp>

// Function prototypes
static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);

static void framebuffer_size_callback(GLFWwindow *window, int width, int height);

// Window dimensions
static constexpr GLuint kWidth  = 800;
static constexpr GLuint kHeight = 600;

extern int RenderEngineMain()
{
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow *window = glfwCreateWindow(kWidth, kHeight, "RenderEngine", nullptr, nullptr);
    R_ASSERT(window != nullptr);

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Load OpenGL functions, gladLoadGL returns the loaded version, 0 on error.
    const auto version = gladLoadGL(glfwGetProcAddress);
    R_ASSERT(version);

    std::cout << "Loaded OpenGL " << GLAD_VERSION_MAJOR(version) << "." << GLAD_VERSION_MINOR(version) << std::endl;

    // Define the viewport dimensions
    glViewport(0, 0, kWidth, kHeight);

    glEnable(GL_DEPTH_TEST);

    /* load shader */
    auto shader_program = LibGcp::MakeShaderFromName("first_vertex_shader", "first_fragment_shader");
    shader_program.Activate();

    /* load model */
    LibGcp::ModelSerializer model_serializer;
    auto modell = model_serializer.LoadModelFromExternalFormat("./models/backpack.zip");

    glm::mat4 model = glm::mat4(1.0f);
    model           = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));

    glm::mat4 view = glm::mat4(1.0f);
    view           = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

    glm::mat4 projection;
    projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

    shader_program.SetMat4("model", model);
    shader_program.SetMat4("view", view);
    shader_program.SetMat4("projection", projection);

    // Game loop
    while (glfwWindowShouldClose(window) == 0) {
        // Check if any events have been activated (key pressed, mouse moved etc.) and call corresponding response
        // functions
        glfwPollEvents();

        // Render
        // Clear the color buffer
        glClearColor(0.2F, 0.3F, 0.3F, 1.0F);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Swap the screen buffers
        glfwSwapBuffers(window);
    }

    // Terminates GLFW, clearing any resources allocated by GLFW.
    glfwTerminate();
    return 0;
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(
    GLFWwindow *window, const int key, [[maybe_unused]] int scancode, const int action, [[maybe_unused]] int mode
)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

void framebuffer_size_callback([[maybe_unused]] GLFWwindow *window, const int width, const int height)
{
    glViewport(0, 0, width, height);
}
