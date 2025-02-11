#include <libcgp/engine/view.hpp>
#include <libcgp/window/window.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <libcgp/mgr/settings_mgr.hpp>

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

void LibGcp::CameraInfo::ConvertYawPitchToVector()
{
    front = glm::normalize(glm::vec3(
        cos(glm::radians(yaw)) * cos(glm::radians(pitch)), sin(glm::radians(pitch)),
        sin(glm::radians(yaw)) * cos(glm::radians(pitch))
    ));
}

void LibGcp::CameraInfo::ConvertVectorToYawPitch()
{
    yaw   = glm::degrees(atan2(front.z, front.x));
    pitch = glm::degrees(asin(front.y));
}

LibGcp::View::View() { SyncProjectionMatrixWithSettings(); }

void LibGcp::View::PrepareViewMatrices(Shader &shader)
{
    shader.SetMat4("view", view_matrix_);
    shader.SetMat4("projection", projection_matrix_);
}

void LibGcp::View::PrepareModelMatrices(Shader &shader, const ObjectPosition &position)
{
    const auto model_matrix = PrepareModelMatrices(position);

    shader.SetMat4("model", model_matrix);
}

glm::mat4 LibGcp::View::PrepareModelMatrices(const ObjectPosition &position)
{
    glm::mat4 model_matrix = glm::mat4(1.0F);
    model_matrix           = glm::translate(model_matrix, position.position);
    model_matrix           = glm::rotate(model_matrix, position.rotation.x, glm::vec3(1.0F, 0.0F, 0.0F));
    model_matrix           = glm::rotate(model_matrix, position.rotation.y, glm::vec3(0.0F, 1.0F, 0.0F));
    model_matrix           = glm::rotate(model_matrix, position.rotation.z, glm::vec3(0.0F, 0.0F, 1.0F));
    model_matrix           = glm::scale(model_matrix, position.scale);

    return model_matrix;
}

glm::mat4 LibGcp::View::PrepareRotMatrix(const ObjectPosition &position)
{
    glm::mat4 model_matrix = glm::rotate(glm::mat4(1.0f), position.rotation.x, glm::vec3(1.0F, 0.0F, 0.0F));
    model_matrix           = glm::rotate(model_matrix, position.rotation.y, glm::vec3(0.0F, 1.0F, 0.0F));
    model_matrix           = glm::rotate(model_matrix, position.rotation.z, glm::vec3(0.0F, 0.0F, 1.0F));
    return model_matrix;
}

void LibGcp::View::UpdateCameraPosition()
{
    assert(camera_object_info_ != nullptr && "Camera info is not set");

    const auto pos    = camera_object_info_->position;
    const auto center = pos + camera_object_info_->front;
    const auto up     = camera_object_info_->up;
    view_matrix_      = glm::lookAt(pos, center, up);
}

void LibGcp::View::SetWindowAspectRatio(const float aspect_ratio)
{
    projection_matrix_ = glm::perspective(glm::radians(45.0F), aspect_ratio, 0.1F, 1000000.0F);
}

void LibGcp::View::SyncProjectionMatrixWithSettings()
{
    projection_matrix_ = glm::perspective(
        glm::radians(SettingsMgr::GetInstance().GetSetting<Setting::kFov, float>()),
        Window::GetInstance().GetAspectRatio(), SettingsMgr::GetInstance().GetSetting<Setting::kNear, float>(),
        SettingsMgr::GetInstance().GetSetting<Setting::kFar, float>()
    );
}
