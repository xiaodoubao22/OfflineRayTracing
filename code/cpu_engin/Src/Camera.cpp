#include "Camera.h"

namespace CpuEngin
{

void Camera::SetViewMatrix(const glm::mat4& view) {
    mViewMatrix = view;
}

void Camera::SetViewMatrix(glm::vec3 eye, glm::vec3 center, glm::vec3 up) {
    mViewMatrix = glm::lookAt(eye, center, up);
}

}



