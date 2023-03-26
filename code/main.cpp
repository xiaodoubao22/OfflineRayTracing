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
#include "TexureSampler.h"

#include <algorithm>
#include <omp.h> 
#include <vector>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"


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

    for (int i = 0; i < Consts::HEIGHT; i++)
    {
        for (int j = 0; j < Consts::WIDTH; j++)
        {
            *p++ = (unsigned char)std::clamp(powf(*S++, Consts::GAMA) * 255.0f, 0.0f, 255.0f);  // R 通道
            *p++ = (unsigned char)std::clamp(powf(*S++, Consts::GAMA) * 255.0f, 0.0f, 255.0f);  // G 通道
            *p++ = (unsigned char)std::clamp(powf(*S++, Consts::GAMA) * 255.0f, 0.0f, 255.0f);  // B 通道
        }
    }

    stbi_write_png("../../result/image.png", Consts::WIDTH, Consts::HEIGHT, 3, image, 0);
}

#include "LdsGenerator.h"

int main() {
    std::cout << "start pt" << std::endl;

    Scene* scene = new Scene;

    int width, height, nrChannels;
    unsigned char* data;
    float* texBuffer;
    stbi_set_flip_vertically_on_load(true);
    data = stbi_load("../../models/sphere/chessBoard.jpg", &width, &height, &nrChannels, 0);
    texBuffer = new float[width * height * nrChannels];
    for (int i = 0; i < width * height * nrChannels; i++) {
        texBuffer[i] = (float)data[i] / 255.0f;
    }
    delete data;
    TexureSampler3F* tBoxDefuse = new TexureSampler3F(texBuffer, width, height);
    scene->PushTexure(tBoxDefuse);

    stbi_set_flip_vertically_on_load(true);
    data = stbi_load("../../models/sphere/world-roughness.png", &width, &height, &nrChannels, 0);
    texBuffer = new float[width * height * nrChannels];
    for (int i = 0; i < width * height * nrChannels; i++) {
        texBuffer[i] = (float)data[i] / 255.0f;
    }
    delete data;
    TexureSampler1F* tWorldRoughness = new TexureSampler1F(texBuffer, width, height);
    scene->PushTexure(tWorldRoughness);

    stbi_set_flip_vertically_on_load(true);
    data = stbi_load("../../models/sphere/world-roughnessInv.png", &width, &height, &nrChannels, 0);
    texBuffer = new float[width * height * nrChannels];
    for (int i = 0; i < width * height * nrChannels; i++) {
        texBuffer[i] = (float)data[i] / 255.0f;
    }
    delete data;
    TexureSampler1F* tWorldRoughnessInv = new TexureSampler1F(texBuffer, width, height);
    scene->PushTexure(tWorldRoughnessInv);

    stbi_set_flip_vertically_on_load(true);
    texBuffer = stbi_loadf("../../models/sphere/Alexs_Apt_2k.hdr", &width, &height, &nrChannels, 0);
    SphericalMap* enviromentLightMap = new SphericalMap(texBuffer, width, height);
    scene->PushmLightMap(enviromentLightMap);



    MaterialDefuse* whiteEmission = new MaterialDefuse(WHITE);
    whiteEmission->mHasEmission = true;
    whiteEmission->mEmissionRadiance = glm::vec3(25.0f);
    scene->PushMaterial(whiteEmission);

    MaterialSpecular* whiteSpecular = new MaterialSpecular();
    MaterialSpecular* cyanSpecular = new MaterialSpecular(CYAN);
    MaterialSpecular* redSpecular = new MaterialSpecular(RED);
    scene->PushMaterial(whiteSpecular);
    scene->PushMaterial(cyanSpecular);
    scene->PushMaterial(redSpecular);

    MaterialTransparent* whiteTransparent = new MaterialTransparent(1.5f);
    MaterialTransparent* orangeTransparent = new MaterialTransparent(1.5f, glm::vec3(1.0f, 0.85f, 0.0f));
    MaterialCookTorrance* GoldGlossy = new MaterialCookTorrance(0.1f, glm::vec3(1.0f, 0.86f, 0.57f));
    MaterialCookTorrance* WhiteGlossy = new MaterialCookTorrance(0.3f, glm::vec3(1.0f));
    MaterialFrostedGlass* FrostedGlass = new MaterialFrostedGlass(0.01f, 1.5f);
    scene->PushMaterial(whiteTransparent);
    scene->PushMaterial(orangeTransparent);
    scene->PushMaterial(GoldGlossy);
    scene->PushMaterial(WhiteGlossy);
    scene->PushMaterial(FrostedGlass);

    MaterialDefuse* whiteDefuse = new MaterialDefuse(WHITE);
    MaterialDefuse* blueDefuse = new MaterialDefuse(BLUE);
    MaterialDefuse* redDefuse = new MaterialDefuse(RED);
    MaterialDefuse* cyanDefuse = new MaterialDefuse(CYAN);
    MaterialDefuse* yellowDefuse = new MaterialDefuse(YELLOW);
    scene->PushMaterial(whiteDefuse);
    scene->PushMaterial(blueDefuse);
    scene->PushMaterial(redDefuse);
    scene->PushMaterial(cyanDefuse);
    scene->PushMaterial(yellowDefuse);

    
    MaterialDefuse* boxDefuse = new MaterialDefuse(tBoxDefuse);
    MaterialCookTorrance* worldMapGlossy = new MaterialCookTorrance(tWorldRoughness, glm::vec3(1.0f, 0.86f, 0.57f));
    MaterialFrostedGlass* worldMapGlass = new MaterialFrostedGlass(tWorldRoughnessInv, 1.5f);
    scene->PushMaterial(boxDefuse);
    scene->PushMaterial(worldMapGlossy);
    scene->PushMaterial(worldMapGlass);

    std::string pFile = "../../models/dragon/dragon.obj";
    std::vector<Mesh*> dragonMeshes = Scene::LoadModel(pFile, GoldGlossy, 200.0f);
    for (auto mesh : dragonMeshes) {
        glm::mat4 meshModel = glm::rotate(glm::mat4(1.0f), -Consts::PI * 0.5f, glm::vec3(1.0f, 0.0f, 0.0f));
        meshModel = glm::translate(meshModel, -glm::vec3(-100.0f, 0.0f, -42.0f));
        mesh->SetModelMatrix(meshModel);
        scene->PushShape(mesh);
    }

    pFile = "../../models/teaport/teaport.obj";
    std::vector<Mesh*> teaportMeshes = Scene::LoadModel(pFile, whiteTransparent, 1.2f);
    for (auto mesh : teaportMeshes) {
        glm::mat4 meshModel = glm::rotate(glm::mat4(1.0f), -Consts::PI * 0.5f, glm::vec3(1.0f, 0.0f, 0.0f));
        meshModel = glm::rotate(meshModel, -Consts::PI * 0.5f, glm::vec3(0.0f, 0.0f, 1.0f));
        meshModel = glm::translate(meshModel, -glm::vec3(70.0f, -100.0f, -98.0f));
        mesh->SetModelMatrix(meshModel);
        scene->PushShape(mesh);
    }

    pFile = "../../models/buddha/buddha.obj";
    std::vector<Mesh*> buddhaMeshes = Scene::LoadModel(pFile, orangeTransparent, 150.0f);
    for (auto mesh : buddhaMeshes) {
        glm::mat4 meshModel = glm::rotate(glm::mat4(1.0f), -Consts::PI * 0.5f, glm::vec3(1.0f, 0.0f, 0.0f));
        meshModel = glm::rotate(meshModel, -Consts::PI * 1.25f, glm::vec3(0.0f, 0.0f, 1.0f));
        meshModel = glm::translate(meshModel, -glm::vec3(100.0f, 100.0f, -32.0f));
        mesh->SetModelMatrix(meshModel);
        scene->PushShape(mesh);
    }

    pFile = "../../models/bunny/bunny.obj";
    std::vector<Mesh*> bunnyMeshes = Scene::LoadModel(pFile, WhiteGlossy, 40.0f);
    for (auto mesh : bunnyMeshes) {
        glm::mat4 meshModel = glm::rotate(glm::mat4(1.0f), -Consts::PI * 0.5f, glm::vec3(1.0f, 0.0f, 0.0f));
        meshModel = glm::rotate(meshModel, -Consts::PI * 0.5f, glm::vec3(0.0f, 0.0f, 1.0f));
        meshModel = glm::translate(meshModel, -glm::vec3(160.0f, 20.0f, -101.0f));
        mesh->SetModelMatrix(meshModel);
        scene->PushShape(mesh);
    }

    // 发光物
    //scene->PushShape(new Triangle(glm::vec3(0.4, 0.99, 0.4)* Consts::SCALE, glm::vec3(-0.4, 0.99, 0.4)* Consts::SCALE, glm::vec3(-0.4, 0.99, -0.4)* Consts::SCALE, whiteEmission));
    //scene->PushShape(new Triangle(glm::vec3(0.4, 0.99, 0.4)* Consts::SCALE, glm::vec3(-0.4, 0.99, -0.4)* Consts::SCALE, glm::vec3(0.4, 0.99, -0.4)* Consts::SCALE, whiteEmission));
    // 背景盒子
    // bottom
    //scene->PushShape(new Triangle(glm::vec3(1, -1, 1) * Consts::SCALE, glm::vec3(-1, -1, -1) * Consts::SCALE, glm::vec3(-1, -1, 1) * Consts::SCALE, whiteDefuse));
    //scene->PushShape(new Triangle(glm::vec3(1, -1, 1) * Consts::SCALE, glm::vec3(1, -1, -1) * Consts::SCALE, glm::vec3(-1, -1, -1) * Consts::SCALE, whiteDefuse));
    //// top
    //scene->PushShape(new Triangle(glm::vec3(1, 1, 1) * Consts::SCALE, glm::vec3(-1, 1, 1) * Consts::SCALE, glm::vec3(-1, 1, -1) * Consts::SCALE, whiteDefuse));
    //scene->PushShape(new Triangle(glm::vec3(1, 1, 1) * Consts::SCALE, glm::vec3(-1, 1, -1) * Consts::SCALE, glm::vec3(1, 1, -1) * Consts::SCALE, whiteDefuse));
    //// back
    ////scene->PushShape(new Triangle(glm::vec3(1, -1, -1) * Consts::SCALE, glm::vec3(-1, 1, -1) * Consts::SCALE, glm::vec3(-1, -1, -1) * Consts::SCALE, whiteDefuse));
    ////scene->PushShape(new Triangle(glm::vec3(1, -1, -1) * Consts::SCALE, glm::vec3(1, 1, -1) * Consts::SCALE, glm::vec3(-1, 1, -1) * Consts::SCALE, whiteDefuse));
    //scene->PushShape(new Triangle(glm::vec3(1, -1, -1) * Consts::SCALE, glm::vec3(-1, 1, -1) * Consts::SCALE, glm::vec3(-1, -1, -1) * Consts::SCALE, 
    //                              glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f),
    //                              glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 1.0f), boxDefuse));
    //scene->PushShape(new Triangle(glm::vec3(1, -1, -1) * Consts::SCALE, glm::vec3(1, 1, -1) * Consts::SCALE, glm::vec3(-1, 1, -1) * Consts::SCALE,
    //                              glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f),
    //                              glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 0.0f), glm::vec2(0.0f, 0.0f), boxDefuse));
    //// left
    //scene->PushShape(new Triangle(glm::vec3(-1, -1, -1) * Consts::SCALE, glm::vec3(-1, 1, 1) * Consts::SCALE, glm::vec3(-1, -1, 1) * Consts::SCALE, blueDefuse));
    //scene->PushShape(new Triangle(glm::vec3(-1, -1, -1) * Consts::SCALE, glm::vec3(-1, 1, -1) * Consts::SCALE, glm::vec3(-1, 1, 1) * Consts::SCALE, blueDefuse));
    //// right
    //scene->PushShape(new Triangle(glm::vec3(1, 1, 1) * Consts::SCALE, glm::vec3(1, -1, -1) * Consts::SCALE, glm::vec3(1, -1, 1) * Consts::SCALE, redDefuse));
    //scene->PushShape(new Triangle(glm::vec3(1, -1, -1) * Consts::SCALE, glm::vec3(1, 1, 1) * Consts::SCALE, glm::vec3(1, 1, -1) * Consts::SCALE, redDefuse));
    // yellow
    //scene->PushShape(new Triangle(glm::vec3(0.5, -0.5, -0.99) * Consts::SCALE, glm::vec3(-0.5, 0.5, -0.99) * Consts::SCALE, glm::vec3(-0.5, -0.5, -0.99) * Consts::SCALE, cyanDefuse));
    //scene->PushShape(new Triangle(glm::vec3(0.5, -0.5, -0.99) * Consts::SCALE, glm::vec3(0.5, 0.5, -0.99) * Consts::SCALE, glm::vec3(-0.5, 0.5, -0.99) * Consts::SCALE, cyanDefuse));

    //scene->PushShape(new Triangle(glm::vec3(0.5, -0.5, 0.9) * Consts::SCALE, glm::vec3(-0.5, 0.5, 0.9) * Consts::SCALE, glm::vec3(-0.5, -0.5, 0.9) * Consts::SCALE, FrostedGlass));
    //scene->PushShape(new Triangle(glm::vec3(0.5, -0.5, 0.9) * Consts::SCALE, glm::vec3(0.5, 0.5, 0.9) * Consts::SCALE, glm::vec3(-0.5, 0.5, 0.9) * Consts::SCALE, FrostedGlass));
    //scene->PushShape(new Triangle(glm::vec3(0.5, -0.5, 0.85) * Consts::SCALE, glm::vec3(-0.5, -0.5, 0.85) * Consts::SCALE, glm::vec3(-0.5, 0.5, 0.85) * Consts::SCALE, FrostedGlass));
    //scene->PushShape(new Triangle(glm::vec3(0.5, -0.5, 0.85) * Consts::SCALE, glm::vec3(-0.5, 0.5, 0.85) * Consts::SCALE, glm::vec3(0.5, 0.5, 0.85) * Consts::SCALE, FrostedGlass));

    //scene->PushShape(new Sphere(glm::vec3(-0.75, -0.8, 0.6)* Consts::SCALE, 0.2f * Consts::SCALE, whiteSpecular));
    //scene->PushShape(new Sphere(glm::vec3(-0.3, -0.7, 0.2) * Consts::SCALE, 0.3f * Consts::SCALE, redSpecular));
    //scene->PushShape(new Sphere(glm::vec3(0.4, -0.6, -0.5)* Consts::SCALE, 0.4f * Consts::SCALE, GoldGlossy));
    //scene->PushShape(new Sphere(glm::vec3(0.55, -0.75, 0.7)* Consts::SCALE, 0.25f * Consts::SCALE, FrostedGlass));
    //scene->PushShape(new Sphere(glm::vec3(0.0, -0.6, 0.0) * Consts::SCALE, 0.4f * Consts::SCALE, whiteTransparent));

    //scene->PushShape(new Sphere(glm::vec3(0.0, -0.4, 0.0) * Consts::SCALE, 0.6f * Consts::SCALE, FrostedGlass));

    // 棋盘格
    Triangle* chessBoardA = new Triangle(glm::vec3(2.0, -2.0, -1) * Consts::SCALE, glm::vec3(-2.0, 2.0, -1) * Consts::SCALE, glm::vec3(-2.0, -2.0, -1) * Consts::SCALE, boxDefuse);
    chessBoardA->SetNormal(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    chessBoardA->SetTexureCoord(glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 1.0f));

    Triangle* chessBoardB = new Triangle(glm::vec3(2.0, -2.0, -1) * Consts::SCALE, glm::vec3(2.0, 2.0, -1) * Consts::SCALE, glm::vec3(-2.0, 2.0, -1) * Consts::SCALE, boxDefuse);
    chessBoardB->SetNormal(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    chessBoardB->SetTexureCoord(glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 0.0f), glm::vec2(0.0f, 0.0f));

    scene->PushShape(chessBoardA);
    scene->PushShape(chessBoardB);

    // 图像
    float* image = new float[Consts::WIDTH * Consts::HEIGHT * 3];
    memset(image, 0, sizeof(float) * Consts::WIDTH * Consts::HEIGHT * 3);

    glm::mat4 view = glm::lookAt(glm::vec3(3.5f, 0.0f, 0.5f), glm::vec3(2.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    //glm::mat4 view = glm::lookAt(glm::vec3(4.0f, 0.0f, -1.01f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    Randerer randerer;
    randerer.SetViewMatrix(view);
    randerer.Draw(image, *scene);

    imshow(image);

	return 0;

}


int main0() {



    //int width, height, nrChannels;
    //float* data = stbi_loadf("../models/sphere/world-mask.png", &width, &height, &nrChannels, 0);
    //TexureSampler3F t0(data, width, height);

    //unsigned char* cBuffer = new unsigned char[width * height * nrChannels];
    //unsigned char* p = cBuffer;
    //float* s = data;
    ////std::cout << nrChannels << std::endl;
    //for (int i = 0; i < width * height * nrChannels; i++) {
    //    if ((*s++) < 0.01) {
    //        *p++ = (unsigned char)(0.05f * 255.0f);
    //        //*p++ = (unsigned char)(1.0f * 255.0f);
    //        //*p++ = (unsigned char)(1.0f * 255.0f);
    //    }
    //    else {
    //        *p++ = (unsigned char)(0.5f * 255.0f);
    //        //*p++ = (unsigned char)(0.6f * 255.0f);
    //        //*p++ = (unsigned char)(0.6f * 255.0f);
    //    }
    //}

    //stbi_write_png("../models/sphere/world-mask2.png", width, height, nrChannels, cBuffer, 0);
    return 0;
}


