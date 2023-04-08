#ifndef CAMERA_H
#define CAMERA_H
#include <stdint.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace CpuEngin{

class Camera 
{
public:
    int mWidth = 1440;
    int mHeight = 1440;
    float mFovY = 60;
    glm::mat4 mViewMatrix;
    float mGama = 2.2f;
public:
    void SetViewMatrix(const glm::mat4& view);
    void SetViewMatrix(glm::vec3 eye, glm::vec3 center, glm::vec3 up);
};

}

#endif // !CAMERA_H