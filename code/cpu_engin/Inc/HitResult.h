#ifndef HIT_RESULT_H
#define HIT_RESULT_H

#include "Utils.h"
#include "Material.h"

class HitResult
{
public:
    int isHit = 0;             // 是否命中(1为命中，其他值未命中)
    float distance = FLT_MAX;         // 与交点的距离
    glm::vec3 hitPoint = glm::vec3(0.0f, 0.0f, 0.0f);  // 光线命中点
    Material* material = nullptr;              // 命中点的表面材质
    glm::vec3 normal;                // 法向量
    glm::vec2 texCoord;              // 纹理坐标
};

#endif // !HIT_RESULT


