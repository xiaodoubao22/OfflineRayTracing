#include "Renderer.h"
#include "SceneReader.h"
#include <iostream>

int main() {
    std::string floderName = "../../models/TestScene";
    Scene* scene = Hmi::SceneReader::GetInstance()->ReadFromFloder(floderName);
    if (scene == nullptr) {
        return 0;
    }

    std::cout << "main cfg" << scene->mConfigInfo.threadCount << std::endl;

    // 图像
    float* image = new float[scene->mCamera.mWidth * scene->mCamera.mHeight * 3];
    memset(image, 0, sizeof(float) * scene->mCamera.mWidth * scene->mCamera.mHeight * 3);

    CpuEngin::Renderer renderer;
    renderer.Draw(image, *scene);

    Utils::SaveImage(image, std::string("../../result/image.png"), scene->mCamera.mWidth, scene->mCamera.mHeight, 3);

}

