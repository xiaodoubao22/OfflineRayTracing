#include "Utils.h"
#include "stb/stb_image.h"
#include "stb/stb_image_write.h"

std::random_device dev;
float Utils::GetUniformRandom(float min, float max) {
    std::mt19937 rng(dev());
    std::uniform_real_distribution<float> dist(min, max); // distribution in range [min, max]
    return dist(rng);
}

// 单位球内的随机向量
glm::vec3 Utils::randomVec3()
{
    float z = GetUniformRandom(-1.0f, 1.0f);
    float phi = GetUniformRandom(0.0f, 2 * Consts::PI);
    float r = std::sqrt(1.0f - z * z);
    return glm::vec3(r * std::cos(phi), r * std::sin(phi), z);
}

// 法向半球随机向量
glm::vec3 Utils::randomDirection(glm::vec3 n)
{
    glm::vec3 randVec = randomVec3();
    float dotNToRandVec = dot(n, randVec);
    if (dotNToRandVec < 0) {
        randVec = randVec + dotNToRandVec * n;
    }
    return randVec;
}

glm::vec3 Utils::RandomDirectionFromLDS(glm::vec3 n, float x, float y)
{
    float z = 2.0 * (x - 0.5f);
    float phi = y * 2 * Consts::PI;
    float r = std::sqrt(1.0f - z * z);
    glm::vec3 randVec = glm::vec3(r * std::cos(phi), r * std::sin(phi), z);

    float dotNToRandVec = dot(n, randVec);
    if (dotNToRandVec < 0) {
        randVec = randVec + dotNToRandVec * n;
    }
    return randVec;
}

void Utils::SaveImage(float* SRC, const std::string& filePath, int width, int height, int channel)
{
    int imageBufferSize = width * height * channel;
    unsigned char* image = new unsigned char[imageBufferSize];// 图像buffer
    unsigned char* p = image;
    float* S = SRC;    // 源数据
    for (int i = 0; i < imageBufferSize; i++)
    {
        *p++ = (unsigned char)std::clamp((*S++) * 255.0f, 0.0f, 255.0f);
    }
    stbi_write_png(filePath.c_str(), width, height, channel, image, 0);
}
