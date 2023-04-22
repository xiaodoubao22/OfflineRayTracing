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

