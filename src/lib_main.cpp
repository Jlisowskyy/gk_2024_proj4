#include <array>
#include <iostream>

// clang-format off
#include <glad/gl.h>
#include <GLFW/glfw3.h> /* (include after glad) */
// clang-format on

#include <shaders/static_header.hpp>
#include <utils/macros.hpp>

// Function prototypes
static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);

static void framebuffer_size_callback(GLFWwindow *window, int width, int height);

// Window dimensions
static constexpr GLuint kWidth  = 800;
static constexpr GLuint kHeight = 600;

static constexpr std::array kVertices{
    0.5F,  0.5F,  0.0F,  // top right
    0.5F,  -0.5F, 0.0F,  // bottom right
    -0.5F, -0.5F, 0.0F,  // bottom left
    -0.5F, 0.5F,  0.0F,  // top left
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
    VERIFY_OUTPUT_GLFW(window != nullptr);

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Load OpenGL functions, gladLoadGL returns the loaded version, 0 on error.
    const auto version = gladLoadGL(glfwGetProcAddress);
    VERIFY_OUTPUT_GLFW(version);

    std::cout << "Loaded OpenGL " << GLAD_VERSION_MAJOR(version) << "." << GLAD_VERSION_MINOR(version) << std::endl;

    // Define the viewport dimensions
    glViewport(0, 0, kWidth, kHeight);

    // Load VertexShader
    const auto vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &StaticShaders::first_vertex_shader, nullptr);
    glCompileShader(vertex_shader);

    // Check for shader compile errors
    ENSURE_SUCCESS_SHADER_OPENGL(vertex_shader);

    // Load FragmentShader
    const auto fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &StaticShaders::first_fragment_shader, nullptr);
    glCompileShader(fragment_shader);

    // Check for shader compile errors
    ENSURE_SUCCESS_SHADER_OPENGL(fragment_shader);

    // create shader program
    const auto shader_program = glCreateProgram();

    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);

    // Check for linking errors
    ENSURE_SUCESSS_PROGRAM_OPENGL(shader_program);

    // Prepare Vertex Array Object (VAO)
    GLuint VAO{};
    glGenVertexArrays(1, &VAO);

    // Prepare Vertex Buffer Object (VBO)
    GLuint VBO{};
    glGenBuffers(1, &VBO);

    // Prepare Element Buffer Object (EBO)
    GLuint EBO{};
    glGenBuffers(1, &EBO);

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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
    glEnableVertexAttribArray(0);

    // delete shaders
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

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
        glUseProgram(shader_program);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
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
