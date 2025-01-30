#include <libcgp/engine/view.hpp>
#include <libcgp/window/window.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

LibGcp::View::View()
    : view_matrix_(glm::translate(glm::mat4(1.0F), glm::vec3(0.0F, 0.0F, -10.0F))),
      projection_matrix_(glm::perspective(glm::radians(45.0F), Window::GetInstance().GetAspectRatio(), 0.1F, 100.0F))
{
}

void LibGcp::View::ChangeCameraType(const CameraType type) {}

void LibGcp::View::PrepareViewMatrices(Shader &shader)
{
    shader.SetMat4("view", view_matrix_);
    shader.SetMat4("projection", projection_matrix_);
}

void LibGcp::View::PrepareModelMatrices(Shader &shader, const ObjectPosition &position)
{
    glm::mat4 model_matrix = glm::mat4(1.0F);
    model_matrix           = glm::translate(model_matrix, position.position);
    model_matrix           = glm::rotate(model_matrix, position.rotation.x, glm::vec3(1.0F, 0.0F, 0.0F));
    model_matrix           = glm::rotate(model_matrix, position.rotation.y, glm::vec3(0.0F, 1.0F, 0.0F));
    model_matrix           = glm::rotate(model_matrix, position.rotation.z, glm::vec3(0.0F, 0.0F, 1.0F));

    shader.SetMat4("model", model_matrix);
}
