#include "Renderer.h"
#include "SceneReader.h"
#include <iostream>

int main() {
    std::string floderName = "../../models/MoriKnob";
    Scene* scene = Hmi::SceneReader::GetInstance()->ReadFromFloder(floderName);
    if (scene == nullptr) {
        return 0;
    }

    std::cout << "main cfg" << scene->mConfigInfo.threadCount << std::endl;

    // glm::mat4 trans(1.0f);
    // trans = glm::rotate(trans, glm::radians(210.0f), glm::vec3(0.0, 0.0, 1.0));
    // for(int i = 0; i < 4; i++) {
    //     for(int j = 0; j < 4; j++) {
    //         std::cout << trans[i][j] << " ";
    //     }
    //     std::cout << std::endl;
    // }

    // 图像
    float* image = new float[scene->mCamera.mWidth * scene->mCamera.mHeight * 3];
    memset(image, 0, sizeof(float) * scene->mCamera.mWidth * scene->mCamera.mHeight * 3);

    CpuEngin::Renderer renderer;
    renderer.Draw(image, *scene);

    Utils::SaveImage(image, std::string("../../result/image.png"), scene->mCamera.mWidth, scene->mCamera.mHeight, 3);

}

// #include "stb/stb_image.h"
// int main() {


//     int width, height, nChannels;
//     unsigned char* texureData = nullptr;
//     texureData = stbi_load("../../result/image0_8.png", &width, &height, &nChannels, 0);
//     // 转换格式
//     float* texureBuffer1 = new float[width * height * nChannels];
//     for (int i = 0; i < width * height * nChannels; i++) {
//         texureBuffer1[i] = (float)texureData[i] / 255.0f;
//     }
//     std::cout << "1111111111" << std::endl;

//     delete[] texureData;

//     std::cout << "222222222222" << std::endl;

//     texureData = stbi_load("../../result/image08.png", &width, &height, &nChannels, 0);
//     // 转换格式
//     float* texureBuffer2 = new float[width * height * nChannels];
//     for (int i = 0; i < width * height * nChannels; i++) {
//         texureBuffer2[i] = (float)texureData[i] / 255.0f;
//     }
// std::cout << "33333333333" << std::endl;
//     for (int i = 0; i < width * height * nChannels; i++) {
//         texureBuffer2[i] = (texureBuffer1[i] + texureBuffer2[i]) * 0.5f;
//     }
// std::cout << width << " " << height << " " << nChannels << std::endl;
//     Utils::SaveImage(texureBuffer2, std::string("../../result/imagennn.png"), width, height, 3);
//     std::cout << "55555555555" << std::endl;

// }