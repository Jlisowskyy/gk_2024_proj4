#include <libcgp/engine/view.hpp>
#include <libcgp/window/window.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void LibGcp::CameraInfo::MoveFreeCamera(const float distance, const buttons_t &buttons)
{
    if (buttons[GLFW_KEY_W]) {
        position += distance * front;
    }

    if (buttons[GLFW_KEY_S]) {
        position -= distance * front;
    }

    if (buttons[GLFW_KEY_A]) {
        position -= distance * glm::normalize(glm::cross(front, up));
    }

    if (buttons[GLFW_KEY_D]) {
        position += distance * glm::normalize(glm::cross(front, up));
    }
}

LibGcp::View::View()
    : projection_matrix_(glm::perspective(glm::radians(45.0F), Window::GetInstance().GetAspectRatio(), 0.1F, 100.0F))
{
    /* ensure that camera is initialized */
    camera_type_ = CameraType::kLast;
    ChangeCameraType(CameraType::kStatic);
}

void LibGcp::View::ChangeCameraType(const CameraType type)
{
    assert(type != CameraType::kLast);

    if (camera_type_ == type) {
        return;
    }

    camera_type_ = type;
    if (camera_type_ == CameraType::kStatic) {
        camera_info_.position = glm::vec3(0.0f, 0.0f, 30.0f);
        camera_info_.front    = glm::vec3(0.0f, 0.0f, -1.0f);
        camera_info_.up       = glm::vec3(0.0f, 1.0f, 0.0f);
        UpdateViewMatrix_();
    }
}

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

void LibGcp::View::UpdateCameraPosition()
{
    if (camera_type_ != CameraType::kStatic) {
        camera_info_ = *camera_object_info_;
    }

    UpdateViewMatrix_();
}

void LibGcp::View::UpdateViewMatrix_()
{
    view_matrix_ = glm::lookAt(camera_info_.position, camera_info_.position + camera_info_.front, camera_info_.up);
}
