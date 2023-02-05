#include "Utils.h"
#include "Shape.h"
#include "Triangle.h"
#include "Sphere.h"
#include "Randerer.h"
#include "Scene.h"
#include "Material.h"
#include "MaterialDefuse.h"
#include "MaterialSpecular.h"
#include "MaterialTransparent.h"
#include "MaterialCookTorrance.h"
#include "MaterialFrostedGlass.h"
#include "HitResult.h"

#include "svpng/svpng.inc"

#include <algorithm>
#include <omp.h> 
#include <vector>
#include <iostream>

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>


glm::vec3 RED(1.0f, 0.5f, 0.5f);
glm::vec3 BLUE(0.5f, 0.5f, 1.0f);
glm::vec3 WHITE(1.0f, 1.0f, 1.0f);
glm::vec3 CYAN(0.0f, 1.0f, 1.0f);
glm::vec3 GRAY(0.5f, 0.5f, 0.5f);
glm::vec3 YELLOW(1.0f, 1.0f, 0.0f);

// 输出 SRC 数组中的数据到图像
inline void imshow(float* SRC)
{
    unsigned char* image = new unsigned char[Consts::WIDTH * Consts::HEIGHT * 3];// 图像buffer
    unsigned char* p = image;
    float* S = SRC;    // 源数据

    FILE* fp;
    fopen_s(&fp, "../result/image.png", "wb");

    for (int i = 0; i < Consts::HEIGHT; i++)
    {
        for (int j = 0; j < Consts::WIDTH; j++)
        {
            *p++ = (unsigned char)std::clamp(powf(*S++, Consts::GAMA) * 255.0f, 0.0f, 255.0f);  // R 通道
            *p++ = (unsigned char)std::clamp(powf(*S++, Consts::GAMA) * 255.0f, 0.0f, 255.0f);  // G 通道
            *p++ = (unsigned char)std::clamp(powf(*S++, Consts::GAMA) * 255.0f, 0.0f, 255.0f);  // B 通道
        }
    }

    svpng(fp, Consts::WIDTH, Consts::HEIGHT, image, 0);
}


int main() {
    Scene* scene = new Scene;

    MaterialDefuse* whiteEmission = new MaterialDefuse(WHITE);
    whiteEmission->mHasEmission = true;
    whiteEmission->mEmissionRadiance = glm::vec3(15.0f);

    MaterialSpecular* whiteSpecular = new MaterialSpecular();
    MaterialSpecular* cyanSpecular = new MaterialSpecular(CYAN);
    MaterialSpecular* redSpecular = new MaterialSpecular(RED);

    MaterialTransparent* whiteTransparent = new MaterialTransparent(1.5f);
    MaterialCookTorrance* GoldGlossy = new MaterialCookTorrance(0.3f, glm::vec3(1.00f, 0.86f, 0.57f));
    MaterialFrostedGlass* FrostedGlass = new MaterialFrostedGlass(0.05f, 1.5f, glm::vec3(1.00f, 0.86f, 0.57f));

    MaterialDefuse* whiteDefuse = new MaterialDefuse(WHITE);
    MaterialDefuse* blueDefuse = new MaterialDefuse(BLUE);
    MaterialDefuse* redDefuse = new MaterialDefuse(RED);
    MaterialDefuse* cyanDefuse = new MaterialDefuse(CYAN);
    MaterialDefuse* yellowDefuse = new MaterialDefuse(YELLOW);

    //std::string pFile = "D:/LxyLearn/CGnote/PathTracing/models/sphere/sphere-cubecoords.obj";
    //std::vector<Mesh*> meshes = Scene::LoadModel(pFile, whiteTransparent);
    //for (auto mesh : meshes) {
    //    scene->Push(mesh);
    //}


    // 发光物
    scene->Push(new Triangle(glm::vec3(0.4, 0.99, 0.4)* Consts::SCALE, glm::vec3(-0.4, 0.99, 0.4)* Consts::SCALE, glm::vec3(-0.4, 0.99, -0.4)* Consts::SCALE, whiteEmission));
    scene->Push(new Triangle(glm::vec3(0.4, 0.99, 0.4)* Consts::SCALE, glm::vec3(-0.4, 0.99, -0.4)* Consts::SCALE, glm::vec3(0.4, 0.99, -0.4)* Consts::SCALE, whiteEmission));
    // 背景盒子
    // bottom
    scene->Push(new Triangle(glm::vec3(1, -1, 1) * Consts::SCALE, glm::vec3(-1, -1, -1) * Consts::SCALE, glm::vec3(-1, -1, 1) * Consts::SCALE, whiteDefuse));
    scene->Push(new Triangle(glm::vec3(1, -1, 1) * Consts::SCALE, glm::vec3(1, -1, -1) * Consts::SCALE, glm::vec3(-1, -1, -1) * Consts::SCALE, whiteDefuse));
    // top
    scene->Push(new Triangle(glm::vec3(1, 1, 1) * Consts::SCALE, glm::vec3(-1, 1, 1) * Consts::SCALE, glm::vec3(-1, 1, -1) * Consts::SCALE, whiteDefuse));
    scene->Push(new Triangle(glm::vec3(1, 1, 1) * Consts::SCALE, glm::vec3(-1, 1, -1) * Consts::SCALE, glm::vec3(1, 1, -1) * Consts::SCALE, whiteDefuse));
    // back
    scene->Push(new Triangle(glm::vec3(1, -1, -1) * Consts::SCALE, glm::vec3(-1, 1, -1) * Consts::SCALE, glm::vec3(-1, -1, -1) * Consts::SCALE, cyanDefuse));
    scene->Push(new Triangle(glm::vec3(1, -1, -1) * Consts::SCALE, glm::vec3(1, 1, -1) * Consts::SCALE, glm::vec3(-1, 1, -1) * Consts::SCALE, cyanDefuse));
    // left
    scene->Push(new Triangle(glm::vec3(-1, -1, -1) * Consts::SCALE, glm::vec3(-1, 1, 1) * Consts::SCALE, glm::vec3(-1, -1, 1) * Consts::SCALE, blueDefuse));
    scene->Push(new Triangle(glm::vec3(-1, -1, -1) * Consts::SCALE, glm::vec3(-1, 1, -1) * Consts::SCALE, glm::vec3(-1, 1, 1) * Consts::SCALE, blueDefuse));
    // right
    scene->Push(new Triangle(glm::vec3(1, 1, 1) * Consts::SCALE, glm::vec3(1, -1, -1) * Consts::SCALE, glm::vec3(1, -1, 1) * Consts::SCALE, redDefuse));
    scene->Push(new Triangle(glm::vec3(1, -1, -1) * Consts::SCALE, glm::vec3(1, 1, 1) * Consts::SCALE, glm::vec3(1, 1, -1) * Consts::SCALE, redDefuse));
    // yellow
    scene->Push(new Triangle(glm::vec3(0.1, -0.9, -0.9) * Consts::SCALE, glm::vec3(-0.1, 0.9, -0.9) * Consts::SCALE, glm::vec3(-0.1, -0.9, -0.9) * Consts::SCALE, yellowDefuse));
    scene->Push(new Triangle(glm::vec3(0.1, -0.9, -0.9) * Consts::SCALE, glm::vec3(0.1, 0.9, -0.9) * Consts::SCALE, glm::vec3(-0.1, 0.9, -0.9) * Consts::SCALE, yellowDefuse));

    //scene->Push(new Triangle(glm::vec3(-0.5, 0.5, -0.75) * Consts::SCALE, glm::vec3(0.5, -0.5, -0.75) * Consts::SCALE, glm::vec3(-0.5, -0.5, -0.75) * Consts::SCALE, whiteTransparent));
    //scene->Push(new Triangle(glm::vec3(0.5, 0.5, -0.75) * Consts::SCALE, glm::vec3(0.5, -0.5, -0.75) * Consts::SCALE, glm::vec3(-0.5, 0.5, -0.75) * Consts::SCALE, whiteTransparent));

    //scene->Push(new Triangle(glm::vec3(0.5, -0.5, -0.7) * Consts::SCALE, glm::vec3(-0.5, 0.5, -0.7) * Consts::SCALE, glm::vec3(-0.5, -0.5, -0.7) * Consts::SCALE, whiteTransparent));
    //scene->Push(new Triangle(glm::vec3(0.5, -0.5, -0.7) * Consts::SCALE, glm::vec3(0.5, 0.5, -0.7) * Consts::SCALE, glm::vec3(-0.5, 0.5, -0.7) * Consts::SCALE, whiteTransparent));

    scene->Push(new Sphere(glm::vec3(-0.75, -0.8, 0.6)* Consts::SCALE, 0.2f * Consts::SCALE, whiteSpecular));
    //scene->Push(new Sphere(glm::vec3(-0.3, -0.7, 0.2) * Consts::SCALE, 0.3f * Consts::SCALE, redSpecular));
    //scene->Push(new Sphere(glm::vec3(0.3, -0.6, -0.5)* Consts::SCALE, 0.4f * Consts::SCALE, GoldGlossy));
    scene->Push(new Sphere(glm::vec3(0.5, -0.75, 0.7)* Consts::SCALE, 0.25f * Consts::SCALE, whiteTransparent));


    // 图像
    float* image = new float[Consts::WIDTH * Consts::HEIGHT * 3];
    memset(image, 0, sizeof(float) * Consts::WIDTH * Consts::HEIGHT * 3);
    Randerer randerer;
    randerer.Draw(image, *scene);

    imshow(image);


    delete whiteEmission;
    delete whiteDefuse;
    delete blueDefuse;
    delete redDefuse;
    delete cyanDefuse;
    delete yellowDefuse;
    delete scene;

	return 0;

}


