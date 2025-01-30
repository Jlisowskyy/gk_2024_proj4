#include <array>
#include <cmath>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <libcgp/model.hpp>
#include <libcgp/shader.hpp>
#include <libcgp/utils/macros.hpp>
#include <libcgp/window/window.hpp>

extern int RenderEngineMain()
{
    /* create window */
    LibGcp::Window window{};
    window.Init();

    /* load shader */
    auto shader_program = LibGcp::MakeShaderFromName("first_vertex_shader", "first_fragment_shader");
    shader_program.Activate();

    /* load model */
    LibGcp::ModelSerializer model_serializer;
    auto modell = model_serializer.LoadModelFromExternalFormat("./models/backpack/backpack.obj");

    // glm::mat4 model = glm::mat4(1.0f);
    // model           = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    //
    // glm::mat4 view = glm::mat4(1.0f);
    // view           = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
    //
    // glm::mat4 projection;
    // projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
    //
    // shader_program.SetMat4("model", model);
    // shader_program.SetMat4("view", view);
    // shader_program.SetMat4("projection", projection);

    /* render loop */
    window.RunLoop();
    return 0;
}
