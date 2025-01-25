#include <array>
#include <cmath>
#include <iostream>

// clang-format off
#include <glad/gl.h>
#include <GLFW/glfw3.h> /* (include after glad) */
// clang-format on

#include <libcgp/shader.hpp>
#include <libcgp/texture.hpp>
#include <libcgp/utils/macros.hpp>

// Function prototypes
static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);

static void framebuffer_size_callback(GLFWwindow *window, int width, int height);

// Window dimensions
static constexpr GLuint kWidth  = 800;
static constexpr GLuint kHeight = 600;

static constexpr std::array kVertices{
    // positions // colors // texture coords
    0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,  // top right
    0.5f,  -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,  // bottom right
    -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,  // bottom left
    -0.5f, 0.5f,  0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f   // top left
};

static constexpr std::array kIndices{
    0U, 1U, 3U,  // first triangle
    1U, 2U, 3U,  // second triangle
};

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

    // Prepare Vertex Array Object (VAO)
    GLuint VAO{};
    glGenVertexArrays(1, &VAO);

    // Prepare Vertex Buffer Object (VBO)
    GLuint VBO{};
    glGenBuffers(1, &VBO);

    // Prepare Element Buffer Object (EBO)
    GLuint EBO{};
    glGenBuffers(1, &EBO);

    /* load shader */
    auto shader_program = LibGcp::MakeShaderFromName("first_vertex_shader", "first_fragment_shader");

    /* load texture */
    auto box_texture  = LibGcp::MakeTextureFromImage("container");
    auto face_texture = LibGcp::MakeTextureFromImage("awesomeface");

    // Bind VAO
    glBindVertexArray(VAO);

    // Bind VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Bind EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(kIndices), kIndices.data(), GL_STATIC_DRAW);

    // Copy vertices to VBO
    glBufferData(GL_ARRAY_BUFFER, sizeof(kVertices), kVertices.data(), GL_STATIC_DRAW);

    // instruct OpenGL how to interpret the vertex data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), nullptr);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), reinterpret_cast<void *>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), reinterpret_cast<void *>(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    shader_program.Activate();
    shader_program.SetGLint("texture1", 0);
    shader_program.SetGLint("texture2", 1);

    // Game loop
    while (glfwWindowShouldClose(window) == 0) {
        // Check if any events have been activated (key pressed, mouse moved etc.) and call corresponding response
        // functions
        glfwPollEvents();

        // Render
        // Clear the color buffer
        glClearColor(0.2F, 0.3F, 0.3F, 1.0F);
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw the triangle
        shader_program.Activate();

        box_texture.Bind(0);
        face_texture.Bind(1);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, sizeof(kIndices), GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);

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
