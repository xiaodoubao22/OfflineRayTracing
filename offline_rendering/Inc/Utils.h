#ifndef UTILS_H
#define UTILS_H

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <iostream>
#include <random>

#include "assimp/scene.h"

 //#define SAVE_RAY
 #define CLAMP_COLOR

namespace Consts {
    const float M_PI = 3.14159265350f;
    const float EPS = 0.00001f;
    const int WIDTH = 1440;
    const int HEIGHT = 1440;
    const float FOVY = 40.0f;
    const float SCALE = 200.0f;   // 用于测试模型放缩
    const glm::vec3 EYE(0, 0, 4.0f * SCALE);
    const int SPP = 2048;
    const int THREAD_COUNT = 14;
    const float GAMA = 1.0f / 2.2f;
    const int MAX_TRACE_DEPTH = 10;

    const glm::vec2 SAVE_RAY_COORD = glm::vec2(334, 325);
    const int MAX_SAVE_DEPTH = 10;

    const glm::vec3 Z_AXIS = glm::vec3(0.0f, 0.0f, 1.0f);
    const glm::vec3 Y_AXIS = glm::vec3(0.0f, 1.0f, 0.0f);
    
}

namespace Utils {
    float GetUniformRandom(float min = 0.0f, float max = 1.0f);
    glm::vec3 randomVec3();  // 单位球内的随机向量
    glm::vec3 randomDirection(glm::vec3 n);  // 法向半球随机向量
    glm::vec3 RandomDirectionFromLDS(glm::vec3 n, float x, float y);
    inline float DegToRad(float deg);
    inline float RadToDeg(float rad);
    inline glm::vec3 AiVector3DToGlm(aiVector3D& aiVec);
    inline glm::vec2 AiVector2DToGlm(aiVector2D& aiVec);
    inline glm::vec3 GlmVecMin(glm::vec3 a, glm::vec3 b);
    inline glm::vec3 GlmVecMin(glm::vec3 a, glm::vec3 b, glm::vec3 c);
    inline glm::vec3 GlmVecMax(glm::vec3 a, glm::vec3 b);
    inline glm::vec3 GlmVecMax(glm::vec3 a, glm::vec3 b, glm::vec3 c);
    inline glm::vec3 Refract(glm::vec3 I, glm::vec3 N, float eta);

#ifdef SAVE_RAY
    static FILE* mSaveRayPath = nullptr;
#endif // SAVE_RAY
    
};

inline float Utils::DegToRad(float deg) {
    return deg * Consts::M_PI / 180.0f;
}

inline float Utils::RadToDeg(float rad) {
    return rad * 180.0f / Consts::M_PI;
}

inline glm::vec3 Utils::AiVector3DToGlm(aiVector3D& aiVec) {
    return glm::vec3(aiVec.x, aiVec.y, aiVec.z);
}

inline glm::vec2 Utils::AiVector2DToGlm(aiVector2D& aiVec) {
    return glm::vec2(aiVec.x, aiVec.y);
}

inline glm::vec3 Utils::GlmVecMin(glm::vec3 a, glm::vec3 b) {
    return glm::vec3(std::min(a.x, b.x), std::min(a.y, b.y), std::min(a.z, b.z));
}

inline glm::vec3 Utils::GlmVecMin(glm::vec3 a, glm::vec3 b, glm::vec3 c) {
    return glm::vec3(std::min({ a.x, b.x, c.x }), std::min({ a.y, b.y, c.y }), std::min({ a.z, b.z, c.z }));
}

inline glm::vec3 Utils::GlmVecMax(glm::vec3 a, glm::vec3 b) {
    return glm::vec3(std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z));
}

inline glm::vec3 Utils::GlmVecMax(glm::vec3 a, glm::vec3 b, glm::vec3 c) {
    return glm::vec3(std::max({ a.x, b.x, c.x }), std::max({ a.y, b.y, c.y }), std::max({ a.z, b.z, c.z }));
}

inline glm::vec3 Utils::Refract(glm::vec3 I, glm::vec3 N, float eta) {
    float k = 1.0 - eta * eta * (1.0 - dot(N, I) * dot(N, I));
    if (k < 0.0)
        return glm::vec3(0.0f);
    else
        return eta * I - (eta * dot(N, I) + sqrt(k)) * N;
}

struct Ray {
    glm::vec3 origin = glm::vec3(0.0f);
    glm::vec3 direction = glm::vec3(0.0f);
};

struct TraceInfo {
    bool saveRay = false;
    int depth = 0;
    int threadNum;
};

#endif // UTILS_H


