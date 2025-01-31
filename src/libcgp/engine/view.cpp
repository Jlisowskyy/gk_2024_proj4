#include <libcgp/engine/view.hpp>
#include <libcgp/window/window.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <libcgp/utils/macros.hpp>

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
        camera_position_ = glm::vec3(0.0F, 0.0F, 10.0F);
        camera_front_    = glm::vec3(0.0F, 0.0F, -1.0F);
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
    switch (camera_type_) {
        case CameraType::kStatic:
            break;
        case CameraType::kFollow:
        [[fallthrough]]
        case CameraType::kFree:
        [[fallthrough]]
        case CameraType::kFirstPerson:
            camera_position_ = camera_object_position_;
            camera_front_    = camera_object_front_;
            break;
        case CameraType::kThirdPerson:
            NOT_IMPLEMENTED;
            break;
        default:
            R_ASSERT(false && "Unknown camera type");
    }

    UpdateViewMatrix_();
}

void LibGcp::View::UpdateViewMatrix_()
{
    view_matrix_ = glm::lookAt(camera_position_, camera_front_, glm::vec3(0.0F, 1.0F, 0.0F));
}
