#include "SceneReader.h"

#include <QFile>
#include <QFileInfo>
#include <stdio.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"
#include "stb/stb_image_resize.h"

namespace Hmi
{

SceneReader::SceneReader() {
}

SceneReader::~SceneReader() {

}

SceneReader* SceneReader::GetInstance() {
    static SceneReader instance;
    return &instance;
}

Scene* SceneReader::ReadFromFloder(std::string floderName) {
    // 清空
    mEnvMap = {"", nullptr};
    mTexureMap.clear();
    mMaterialMap.clear();
    mShapes.clear();
    mCamera = {false, CpuEngin::Camera()};
    mConfigInfo = {false, CpuEnginConfig()};
    // 放入默认材质
    MaterialDefuse* defaultMaterial = new MaterialDefuse(glm::vec3(1.0, 0.0, 1.0));
    mMaterialMap.insert(std::make_pair(std::string("DefaultMaterial"), defaultMaterial));

    if (floderName.back() != '/') {
        floderName += "/";
    }

    mTexureMap.clear();
    mMaterialMap.clear();
    
    // 打开scene.xml
    std::string sceneXmlPath = floderName + "scene.xml";
    QFile file(sceneXmlPath.c_str());
    if (!file.exists()) {
        std::cout << "not found file!" << std::endl;
        return nullptr;
    }

    // 读取根节点
    QDomDocument SceneDoc;
    int errLine = -1;
    int errColum = -1;
    QString strError = "";
    if (!SceneDoc.setContent(&file, &strError, &errLine, &errColum)) {
        std::cout << "set content failed!" << std::endl;
        std::cout << "error:" << strError.toStdString() << " r=" << errLine << " c=" << errColum << std::endl;
        return nullptr;
    }
    QDomElement sceneDocRoot = SceneDoc.documentElement();

    // 处理根节点
    mTexureMap.clear();
    ProcessSceneRoot(floderName, sceneDocRoot);

    // 装填scene
    Scene* res = new Scene();
    res->mCamera = mCamera.second;
    res->mConfigInfo = mConfigInfo.second;
    res->PushmLightMap(mEnvMap.second);
    for (auto texure : mTexureMap) {
        res->PushTexure(texure.second);
    }
    for (auto mat : mMaterialMap) {
        res->PushMaterial(mat.second);
    }
    for (auto shape : mShapes) {
        res->PushShape(shape);
    }

    // 清空
    mEnvMap = {"", nullptr};
    mTexureMap.clear();
    mMaterialMap.clear();
    mShapes.clear();
    mCamera = {false, CpuEngin::Camera()};
    mConfigInfo = {false, CpuEnginConfig()};

    return res;
}

int SceneReader::ProcessSceneRoot(const std::string& floderName, const QDomNode &sceneDom) {
    if (sceneDom.nodeName().toStdString() != std::string("scene")) {
        std::cout << "not found scene node" << std::endl;
        return -1;
    }

    QDomNodeList sceneDomObjectList = sceneDom.childNodes();
    for (int i = 0; i < sceneDomObjectList.size(); i++) {
        QDomNode sceneDomObject = sceneDomObjectList.at(i);

        if (sceneDomObject.nodeName().toStdString() == std::string("camera")) {
            if (mCamera.first == false) {   // 保证只读取一次camera
                ProcessCamera(floderName, sceneDomObject);
                mCamera.first = true;
            }
        }
        else if (sceneDomObject.nodeName().toStdString() == std::string("config")) {
            if (mConfigInfo.first == false) {   // 保证只读取一次fonfig
                ProcessConfig(floderName, sceneDomObject);
                mConfigInfo.first = true;
            }
        }
        else if (sceneDomObject.nodeName().toStdString() == std::string("material")) {
            ProcessMaterial(floderName, sceneDomObject);
        }
        else if (sceneDomObject.nodeName().toStdString() == std::string("env_map")) {
            ProcessEnvMap(floderName, sceneDomObject);
        }
        else if (sceneDomObject.nodeName().toStdString() == std::string("shape")) {
            ProcessShape(floderName, sceneDomObject);
        }
        else if (sceneDomObject.nodeName().toStdString() == std::string("texure")) {
            ProcessTexure(floderName, sceneDomObject);
        }
    }

    return 0;
}

int SceneReader::ProcessMaterial(const std::string& floderName, const QDomNode &materialDomNode) {
    std::string materialType = FindAttributeByName(materialDomNode, std::string("type"));    // 材质类型
    std::string materialId = FindAttributeByName(materialDomNode, std::string("id"));    // 材质ID
    std::cout << "materialId=" << materialId << std::endl;
    
    // 根据ID判断是否已读取
    if (mMaterialMap.find(materialId) != mMaterialMap.end()) {
        return -1;
    }

    if (materialType == std::string("diffuse")) {
        MaterialDefuse* material = ReadDiffuseMaterial(materialDomNode);
        mMaterialMap.insert(std::make_pair(materialId, material));
    }
    else if (materialType == std::string("specular")) {
        MaterialSpecular* material = ReadSpecularMaterial(materialDomNode);
        mMaterialMap.insert(std::make_pair(materialId, material));
    }
    else if (materialType == std::string("transparent")) {
        MaterialTransparent* material = ReadTransparentMaterial(materialDomNode);
        mMaterialMap.insert(std::make_pair(materialId, material));
    }
    else if (materialType == std::string("cook_torrance")) {
        MaterialCookTorrance* material = ReadCookTorranceMaterial(materialDomNode);
        mMaterialMap.insert(std::make_pair(materialId, material));
    }
    else if (materialType == std::string("frosted_glass")) {
        MaterialFrostedGlass* material = ReadFrostedGlassMaterial(materialDomNode);
        mMaterialMap.insert(std::make_pair(materialId, material));
    } 
    else {
        return -1;
    }

    return 0;
}

int SceneReader::ProcessTexure(const std::string& floderName, const QDomNode &texureDomNode) {
    std::string texureType = FindAttributeByName(texureDomNode, std::string("type"));    // 纹理类型
    std::string texureId = FindAttributeByName(texureDomNode, std::string("id"));    // 纹理ID

    // 根据ID判断是否已读取
    if (mTexureMap.find(texureId) != mTexureMap.end()) {
        return -1;
    }

    // 找到文件名
    QDomNode fileNameDomNode = FindChildDomNodeByName(texureDomNode, std::string("file_name"));
    std::string fileName = ReadStringFromDomNode(fileNameDomNode);
    fileName = floderName + fileName;

    // 根据不同纹理类型读取纹理
    int width, height, nChannels, nChannelsFromFile;
    unsigned char* texureData = nullptr;
    float* texureBuffer = nullptr;
    if (mTexureTypeMap.find(texureType) != mTexureTypeMap.end()) {
        // 读取文件
        nChannels = mTexureTypeMap[texureType];
        stbi_set_flip_vertically_on_load(true);
        texureData = stbi_load(fileName.c_str(), &width, &height, &nChannelsFromFile, 0);
        if(texureData == nullptr) {
            std::cout << "texure path error!" << std::endl;
            return -1;
        }
        if(nChannels != nChannelsFromFile) {
            std::cout << "num of channels error! " << nChannelsFromFile << "!=" << nChannels << std::endl;
            delete[] texureData;
            return -1;
        }

        // 转换格式
        texureBuffer = new float[width * height * nChannels];
        for (int i = 0; i < width * height * nChannels; i++) {
            texureBuffer[i] = (float)texureData[i] / 255.0f;
        }
        delete[] texureData;
        texureData = nullptr;

        // 创建纹理
        if (nChannels == 1) {
            TexureSampler1F* texureSampler = new TexureSampler1F(texureBuffer, width, height);
            mTexureMap.insert(std::make_pair(texureId, texureSampler));

            TexureSampler2D<float>* sampler2D = new TexureSampler2D<float>(width, height);
            sampler2D->SetData(texureBuffer, width, height);
            mTexureMapC1.insert(std::make_pair(texureId, sampler2D));
        }
        else if (nChannels == 2) {
            TexureSampler2F* texureSampler = new TexureSampler2F(texureBuffer, width, height);
            mTexureMap.insert(std::make_pair(texureId, texureSampler));
        }
        else if (nChannels == 3) {
            TexureSampler3F* texureSampler = new TexureSampler3F(texureBuffer, width, height);
            mTexureMap.insert(std::make_pair(texureId, texureSampler));
        }
        else if (nChannels == 4) {
            TexureSampler4F* texureSampler = new TexureSampler4F(texureBuffer, width, height);
            mTexureMap.insert(std::make_pair(texureId, texureSampler));
        }
        else {
            delete[] texureBuffer;
            texureBuffer = nullptr;
            return -1;
        }

        std::cout << "get texure: " 
            << "type=" << texureType << " " 
            << "id=" << texureId << " " 
            << "w=" << width << " " 
            << "h=" << height << " "
            << "channel=" << nChannels << std::endl;
        return 0;
    }
    else {
        std::cout << "unknown texure type!" << std::endl;
        return -1;
    }

    return 0;
}

int SceneReader::ProcessEnvMap(const std::string& floderName, const QDomNode &envMapDomNode) {
    std::string envMapType = FindAttributeByName(envMapDomNode, std::string("type"));    // 纹理类型
    std::string envMapId = FindAttributeByName(envMapDomNode, std::string("id"));    // 纹理ID

    // 根据ID判断是否已读取
    if (mEnvMap.first == envMapId) {
        return -1;
    }

    // 处理子节点
    QDomNode fileNameDomNode = FindChildDomNodeByName(envMapDomNode, std::string("file_name"));
    std::string fileName = ReadStringFromDomNode(fileNameDomNode);
    std::string filePath = floderName + fileName;
    std::cout << "env_fileName " << filePath << std::endl;

    int width, height, nChannels, nChannelsFromFile;
    float* imageBuffer = nullptr;
    if (envMapType == "spherical") {
        QFileInfo figFileInfo(QString(filePath.c_str()));
        std::string imageFileType = figFileInfo.suffix().toStdString();
        stbi_set_flip_vertically_on_load(true);
        if (imageFileType == "hdr") {
            imageBuffer = stbi_loadf(filePath.c_str(), &width, &height, &nChannelsFromFile, 0);
        } else {
            unsigned char* imageData = stbi_load(filePath.c_str(), &width, &height, &nChannelsFromFile, 0);
            imageBuffer = new float[width * height * nChannelsFromFile];
            for (int i = 0; i < width * height * nChannelsFromFile; i++) {
                imageBuffer[i] = (float)imageData[i] / 255.0f;
            }
            delete[] imageData;
        }
        
        if (nChannelsFromFile == 3) {
            SphericalMap* enviromentLightMap = new SphericalMap(imageBuffer, width, height);
            mEnvMap.first = envMapId;
            mEnvMap.second = enviromentLightMap;
            std::cout << "get env_map "
                << "id=" << mEnvMap.first << " "
                << "width=" << width << " "
                << "height=" << height << " "
                << "channels=" << nChannelsFromFile << " "
                << "fileType=" << imageFileType << std::endl;
        }
    }
}

int SceneReader::ProcessShape(const std::string& floderName, const QDomNode &shapeDomNode) {
    std::string shapeType = FindAttributeByName(shapeDomNode, std::string("type"));    // 类型
    std::string shapeId = FindAttributeByName(shapeDomNode, std::string("id"));    // ID

    // 处理子节点
    if (shapeType == std::string("triangle")) {
        Triangle* triangle = ReadTriangleFromDomNode(shapeDomNode);
        if (triangle != nullptr) {
            mShapes.push_back(triangle);
        }
    }
    else if (shapeType == std::string("mesh")) {
        std::vector<Mesh*> meshes = ReadMeshFromDomNode(shapeDomNode, floderName);
        for (auto mesh : meshes) {
            mShapes.push_back(mesh);
        }
    }
    else {
        return -1;
    }

    return 0;
}

int SceneReader::ProcessCamera(const std::string& floderName, const QDomNode &cameraDomNode) {
    // 分辨率
    QDomNode resolutionDomNode = FindChildDomNodeByName(cameraDomNode, std::string("resolution"));
    if (!resolutionDomNode.isNull()) {
        glm::vec2 resolution = ReadGlmVec2FromDomNode(resolutionDomNode);
        mCamera.second.mWidth = resolution.x;
        mCamera.second.mHeight = resolution.y;
        std::cout << "cam resolution = " << mCamera.second.mWidth << " " << mCamera.second.mHeight << std::endl;
    }

    // y方向foy
    QDomNode fovYDomNode = FindChildDomNodeByName(cameraDomNode, std::string("fov_y"));
    if (!fovYDomNode.isNull()) {
        mCamera.second.mFovY = ReadFloatFromDomNode(fovYDomNode);
        std::cout << "cam fovy = " <<  mCamera.second.mFovY << std::endl;
    }

    // view
    QDomNode positionDomNode = FindChildDomNodeByName(cameraDomNode, std::string("position"));
    QDomNode centerDomNode = FindChildDomNodeByName(cameraDomNode, std::string("center"));
    QDomNode upDomNode = FindChildDomNodeByName(cameraDomNode, std::string("up"));
    if (!positionDomNode.isNull() && !centerDomNode.isNull() && !upDomNode.isNull()) {
        glm::vec3 position = ReadGlmVec3FromDomNode(positionDomNode);
        glm::vec3 center = ReadGlmVec3FromDomNode(centerDomNode);
        glm::vec3 up = ReadGlmVec3FromDomNode(upDomNode);
        mCamera.second.SetViewMatrix(position, center, up);
        std::cout << "cam lookat = " 
            << position.x << " " << position.y << " " << position.z << " --- "
            << center.x << " " << center.y << " " << center.z << " --- "
            << up.x << " " << up.y << " " << up.z << std::endl;
    }

    // gama
    QDomNode gamaDomNode = FindChildDomNodeByName(cameraDomNode, std::string("gama"));
    if (!gamaDomNode.isNull()) {
        mCamera.second.mGama = ReadFloatFromDomNode(gamaDomNode);
        std::cout << "cam gama = " << mCamera.second.mGama << std::endl;
    }

    return 0;
}

int SceneReader::ProcessConfig(const std::string& floderName, const QDomNode &configDomNode) {
    // spp
    QDomNode sppDomNode = FindChildDomNodeByName(configDomNode, std::string("spp"));
    if (!sppDomNode.isNull()) {
        mConfigInfo.second.spp = ReadIntFromDomNode(sppDomNode);
        std::cout << "config spp = " <<  mConfigInfo.second.spp << std::endl;
    }

    // thread_count
    QDomNode threadCountDomNode = FindChildDomNodeByName(configDomNode, std::string("thread_count"));
    if (!threadCountDomNode.isNull()) {
        mConfigInfo.second.threadCount = ReadIntFromDomNode(threadCountDomNode);
        std::cout << "config tc = " <<  mConfigInfo.second.threadCount << std::endl;
    }

    // max_depth
    QDomNode maxDepthDomNode = FindChildDomNodeByName(configDomNode, std::string("max_depth"));
    if (!maxDepthDomNode.isNull()) {
        mConfigInfo.second.maxTraceDepth = ReadIntFromDomNode(maxDepthDomNode);
        std::cout << "config depth = " <<  mConfigInfo.second.maxTraceDepth << std::endl;
    }

    mConfigInfo.second.maxTraceDepth = 10;  // 暂时固定为10
    return 0;
}

MaterialDefuse* SceneReader::ReadDiffuseMaterial(const QDomNode &materialDomNode) {
    MaterialDefuse* mat = new MaterialDefuse();
    // emmision
    QDomNode emmisionDomNode = FindChildDomNodeByName(materialDomNode, std::string("emission"));
    if (!emmisionDomNode.isNull()) {
        glm::vec3 emmision = ReadGlmVec3FromDomNode(materialDomNode);
        mat->mHasEmission = true;
        mat->mEmissionRadiance = emmision;
    }

    // albedo
    QDomNode albedoDomNode = FindChildDomNodeByName(materialDomNode, std::string("albedo"));
    if (!albedoDomNode.isNull()) {
        std::string albedoType = FindAttributeByName(albedoDomNode, std::string("type")); // 类型: vec3 / texure
        std::cout << "get MaterialDefuse: " << "albedo_type=" << albedoType << " " ;
        if (albedoType == "vec3") {
            mat->mAlbedo = ReadGlmVec3FromDomNode(albedoDomNode);
            std::cout << "albedo_data=" << mat->mAlbedo.x << " " <<  mat->mAlbedo.y << " " << mat->mAlbedo.z << " ";
        }
        else if (albedoType == "texure") {
            std::string texureIdStr = ReadStringFromDomNode(albedoDomNode);
            std::cout << "texureIdStr=" << texureIdStr;
            if (mTexureMap.find(texureIdStr) != mTexureMap.end()) {
                mat->SetAlbedo(mTexureMap[texureIdStr]);
            } else {
                std::cout << "not found texureId:" << texureIdStr << std::endl;
            }
        }
        std::cout << std::endl;
    }
    return mat;
}

MaterialSpecular* SceneReader::ReadSpecularMaterial(const QDomNode &materialDomNode) {
    MaterialSpecular* mat = new MaterialSpecular();
    // emmision
    QDomNode emmisionDomNode = FindChildDomNodeByName(materialDomNode, std::string("emission"));
    if (!emmisionDomNode.isNull()) {
        glm::vec3 emmision = ReadGlmVec3FromDomNode(materialDomNode);
        mat->mHasEmission = true;
        mat->mEmissionRadiance = emmision;
    }

    // specularRate
    QDomNode specularRateDomNode = FindChildDomNodeByName(materialDomNode, std::string("color"));
    std::string specularRateType("");   // 类型: vec3
    if (!specularRateDomNode.isNull()) {
        specularRateType = FindAttributeByName(specularRateDomNode, std::string("type"));     
        if (specularRateType == "vec3") {
            mat->mSpecularRate = ReadGlmVec3FromDomNode(specularRateDomNode);
        }
        else if (specularRateType == "texure") {

        }
    }
    std::cout << "get MaterialSpecular: " 
        << "color_type=" << specularRateType << " " 
        << "color_data=" << mat->mSpecularRate.x << " " <<  mat->mSpecularRate.y << " " << mat->mSpecularRate.z << " "
        << std::endl;
    return mat;
}

MaterialTransparent* SceneReader::ReadTransparentMaterial(const QDomNode &materialDomNode) {
    MaterialTransparent* mat = new MaterialTransparent();
    // emmision
    QDomNode emmisionDomNode = FindChildDomNodeByName(materialDomNode, std::string("emission"));
    if (!emmisionDomNode.isNull()) {
        glm::vec3 emmision = ReadGlmVec3FromDomNode(materialDomNode);
        mat->mHasEmission = true;
        mat->mEmissionRadiance = emmision;
    }

    // color
    QDomNode colorDomNode = FindChildDomNodeByName(materialDomNode, std::string("color"));
    if (!colorDomNode.isNull()) {
        std::string colorType = FindAttributeByName(colorDomNode, std::string("type"));
        if (colorType == "vec3") {
            mat->mColor = ReadGlmVec3FromDomNode(colorDomNode);
        }
        else if (colorType == "texure") {

        }
    }

    // ior
    QDomNode iorDomNode = FindChildDomNodeByName(materialDomNode, std::string("ior"));
    std::string iorDataStr("");
    if (!iorDomNode.isNull()) {
        mat->mIor = ReadFloatFromDomNode(iorDomNode);
    }
    std::cout << "get MaterialTransparent: " 
        << "color_data=" << mat->mColor.x << " " <<  mat->mColor.y << " " << mat->mColor.z << " "
        << "ior=" << mat->mIor
        << std::endl;
    return mat;
}

MaterialCookTorrance* SceneReader::ReadCookTorranceMaterial(const QDomNode &materialDomNode) {
    MaterialCookTorrance* mat = new MaterialCookTorrance();
    // emmision
    QDomNode emmisionDomNode = FindChildDomNodeByName(materialDomNode, std::string("emission"));
    if (!emmisionDomNode.isNull()) {
        glm::vec3 emmision = ReadGlmVec3FromDomNode(materialDomNode);
        mat->mHasEmission = true;
        mat->mEmissionRadiance = emmision;
    }
    // f0
    QDomNode f0DomNode = FindChildDomNodeByName(materialDomNode, std::string("f0"));
    std::string f0Type("");   // 类型: vec3
    std::string f0DataStr("");   // 值: 数值
    if (!f0DomNode.isNull()) {
        f0Type = FindAttributeByName(f0DomNode, std::string("type"));   
        if (f0Type == "vec3") {
            mat->SetF0(ReadGlmVec3FromDomNode(f0DomNode));
        }
    }
    // roughness
    QDomNode roughnessDomNode = FindChildDomNodeByName(materialDomNode, std::string("roughness"));
    std::string roughnessType("");   // 类型: vec3
    if (!roughnessDomNode.isNull()) {
        roughnessType = FindAttributeByName(roughnessDomNode, std::string("type"));   
        if (roughnessType == "float") {
            mat->SetRoughness(ReadFloatFromDomNode(roughnessDomNode));
        }
        else if (roughnessType == "texure") {
            std::string texureIdStr = ReadStringFromDomNode(roughnessDomNode);
            std::cout << "texureId = " << texureIdStr << std::endl;
            if (mTexureMap.find(texureIdStr) != mTexureMap.end() && 
                mTexureMap[texureIdStr]->NumChannels() == 1) {
                // mat->SetRoughness(mTexureMap[texureIdStr]);
                mat->SetRoughness(*mTexureMapC1[texureIdStr]);
            }
        }
    }

    mat->GenerateKullaCountyMap();

    std::cout << "get MaterialCookTorrance: " 
        << "f0=" << mat->mF0.x << " " <<  mat->mF0.y << " " << mat->mF0.z << " "
        << "roughness=" << mat->mRoughness << " "
        << std::endl;
    return mat;
}

MaterialFrostedGlass* SceneReader::ReadFrostedGlassMaterial(const QDomNode &materialDomNode) {
    MaterialFrostedGlass* mat = new MaterialFrostedGlass();
    // emmision
    QDomNode emmisionDomNode = FindChildDomNodeByName(materialDomNode, std::string("emission"));
    if (!emmisionDomNode.isNull()) {
        glm::vec3 emmision = ReadGlmVec3FromDomNode(materialDomNode);
        mat->mHasEmission = true;
        mat->mEmissionRadiance = emmision;
    }
    
    // ior
    QDomNode iorDomNode = FindChildDomNodeByName(materialDomNode, std::string("ior"));
    std::string iorType("");   // 类型: vec3
    if (!iorDomNode.isNull()) {
        iorType = FindAttributeByName(iorDomNode, std::string("type"));   
        if (iorType == "float") {
            mat->SetIor(ReadFloatFromDomNode(iorDomNode));
        }
    }
    // roughness
    QDomNode roughnessDomNode = FindChildDomNodeByName(materialDomNode, std::string("roughness"));
    std::string roughnessType("");   // 类型: vec3
    if (!roughnessDomNode.isNull()) {
        roughnessType = FindAttributeByName(roughnessDomNode, std::string("type"));   
        if (roughnessType == "float") {
            mat->SetRoughness(ReadFloatFromDomNode(roughnessDomNode));
        }
        else if (roughnessType == "texure") {
            std::string texureIdStr = ReadStringFromDomNode(roughnessDomNode);
            std::cout << "texureId = " << texureIdStr << std::endl;
            if (mTexureMap.find(texureIdStr) != mTexureMap.end() && 
                mTexureMap[texureIdStr]->NumChannels() == 1) {
                mat->SetRoughness(mTexureMap[texureIdStr]);
            }
        }
    }

    mat->GenerateKullaCountyMap();
    
    std::cout << "get MaterialFrostedGlass: " 
        << "ior=" << mat->mIor << " "
        << "roughness=" << mat->mRoughness << " "
        << std::endl;
    return mat;
}

std::vector<Mesh*> SceneReader::ReadMeshFromDomNode(const QDomNode &shapeDomNode, std::string floderName) {
    std::vector<Mesh*> meshes(0);
    // Material* mat = nullptr;
    glm::mat4 model(1.0f);
    float scale = 1.0f;

    // material
    // QDomNode materialDomNode = FindChildDomNodeByName(shapeDomNode, std::string("material"));
    // std::string materialID = ReadStringFromDomNode(materialDomNode);
    // if (mMaterialMap.find(materialID) != mMaterialMap.end()) {
    //     mat = mMaterialMap[materialID];
    // } else {
    //     return meshes;
    // }

    // model matrix
    QDomNode modelMatDomNode = FindChildDomNodeByName(shapeDomNode, std::string("model_mat"));
    model = ReadGlmMat4FromDomNode(modelMatDomNode);

    // sacle
    QDomNode scaleDomNode = FindChildDomNodeByName(shapeDomNode, std::string("scale"));
    scale = ReadFloatFromDomNode(scaleDomNode);
    std::cout << scale << std::endl;

    // obj
    QDomNode objDomNode = FindChildDomNodeByName(shapeDomNode, std::string("obj"));
    std::string objFileName = ReadStringFromDomNode(objDomNode);
    std::string objPath = floderName + objFileName;

    std::cout << objPath << std::endl;
    meshes = LoadModel(objPath, scale);
    for (auto mesh : meshes) {
        mesh->SetModelMatrix(model);
        mShapes.push_back(mesh);
    }
    std::cout << "mShapeNum=" << mShapes.size() << std::endl;

    return meshes;
}

Triangle* SceneReader::ReadTriangleFromDomNode(const QDomNode &shapeDomNode) {
    Material* mat = nullptr;
    glm::mat4 model(1.0f);
    float scale = 1.0f;

    // material
    QDomNode materialDomNode = FindChildDomNodeByName(shapeDomNode, std::string("material"));
    std::string materialID = ReadStringFromDomNode(materialDomNode);
    std::cout << "matid=" << materialID << " ";
    if (mMaterialMap.find(materialID) != mMaterialMap.end()) {
        mat = mMaterialMap[materialID];
    } else {
        std::cout << "mat id " << materialID << " not found!" << std::endl;
        return nullptr;
    }

    // model matrix
    QDomNode modelMatDomNode = FindChildDomNodeByName(shapeDomNode, std::string("model_mat"));
    model = ReadGlmMat4FromDomNode(modelMatDomNode);

    // sacle
    QDomNode scaleDomNode = FindChildDomNodeByName(shapeDomNode, std::string("scale"));
    scale = ReadFloatFromDomNode(scaleDomNode);
    std::cout << "scale=" << scale << std::endl;

    // pnt
    QDomNode p0DomNode = FindChildDomNodeByName(shapeDomNode, std::string("p0"));
    QDomNode p1DomNode = FindChildDomNodeByName(shapeDomNode, std::string("p1"));
    QDomNode p2DomNode = FindChildDomNodeByName(shapeDomNode, std::string("p2"));
    if (p0DomNode.isNull() || p1DomNode.isNull() || p2DomNode.isNull()) {
        return nullptr;
    }
    glm::vec3 p0 = ReadGlmVec3FromDomNode(p0DomNode);
    glm::vec3 p1 = ReadGlmVec3FromDomNode(p1DomNode);
    glm::vec3 p2 = ReadGlmVec3FromDomNode(p2DomNode);
    Triangle* result = new Triangle(p0 * scale, p1 * scale, p2 * scale, mat);
    result->SetModelMatrix(model);
    std::cout << p0.x << " " << p0.y << " " << p0.z << ", "
        << p1.x << " " << p1.y << " " << p1.z << ", "
        << p2.x << " " << p2.y << " " << p2.z << " --- ";

    // normal
    QDomNode n0DomNode = FindChildDomNodeByName(shapeDomNode, std::string("n0"));
    QDomNode n1DomNode = FindChildDomNodeByName(shapeDomNode, std::string("n1"));
    QDomNode n2DomNode = FindChildDomNodeByName(shapeDomNode, std::string("n2"));
    if (!n0DomNode.isNull() && !n1DomNode.isNull() && !n2DomNode.isNull()) {
        glm::vec3 n0 = ReadGlmVec3FromDomNode(n0DomNode);
        glm::vec3 n1 = ReadGlmVec3FromDomNode(n1DomNode);
        glm::vec3 n2 = ReadGlmVec3FromDomNode(n2DomNode);
        result->SetNormal(n0, n1, n2);
        std::cout << result->mN[0].x << " " << result->mN[0].y << " " << result->mN[0].z << ", "
        << result->mN[1].x << " " << result->mN[1].y << " " << result->mN[1].z << ", "
        << result->mN[2].x << " " << result->mN[2].y << " " << result->mN[2].z << " --- ";
    }


    // normal
    QDomNode t0DomNode = FindChildDomNodeByName(shapeDomNode, std::string("t0"));
    QDomNode t1DomNode = FindChildDomNodeByName(shapeDomNode, std::string("t1"));
    QDomNode t2DomNode = FindChildDomNodeByName(shapeDomNode, std::string("t2"));
    if (!t0DomNode.isNull() && !t1DomNode.isNull() && !t2DomNode.isNull()) {
        glm::vec2 t0 = ReadGlmVec2FromDomNode(t0DomNode);
        glm::vec2 t1 = ReadGlmVec2FromDomNode(t1DomNode);
        glm::vec2 t2 = ReadGlmVec2FromDomNode(t2DomNode);
        result->SetTexureCoord(t0, t1, t2);
        std::cout << t0.x << " " << t0.y << ", "
        << t1.x << " " << t1.y <<  ", "
        << t2.x << " " << t2.y ;
    }

    std::cout << std::endl;
    return result;
}

QDomNode SceneReader::FindChildDomNodeByName(const QDomNode& node, const std::string& name) {
    QDomNodeList lstChildren = node.childNodes();
    QString qStrName(name.c_str());
    for (int i = 0; i < lstChildren.size(); i++) {
        QString nodeName = lstChildren.at(i).nodeName();
        if (nodeName == qStrName) {
            return lstChildren.at(i);
        }
    }
    return QDomNode();
}

std::string SceneReader::FindAttributeByName(const QDomNode& node, const std::string& name) {
    if (!node.hasAttributes()) {
        return std::string("");
    }
    QDomNamedNodeMap attributeMap = node.attributes();
    QDomNode attrNode = attributeMap.namedItem(name.c_str());
    return attrNode.nodeValue().toStdString();
}

glm::vec3 SceneReader::StdStringToGlmVec3(const std::string& str) {
    float x = 0.0f, y = 0.0f, z = 0.0f;
    int ret = sscanf_s(str.c_str(), "%f %f %f", &x, &y, &z);
    if (ret != 3) {
        return glm::vec3(1.0f);
    }
    return glm::vec3(x, y, z);
}

glm::vec2 SceneReader::StdStringToGlmVec2(const std::string& str) {
    float x = 0.0f, y = 0.0f;
    int ret = sscanf_s(str.c_str(), "%f %f", &x, &y);
    if (ret != 2) {
        return glm::vec2(1.0f);
    }
    return glm::vec2(x, y);
}

glm::mat4 SceneReader::ReadGlmMat4FromDomNode(const QDomNode& node) {
    glm::mat4 result(1.0f);
    if (node.isNull()) {
        return result;
    }

    std::string matType = FindAttributeByName(node, std::string("type")); 
    if (matType != std::string("mat4")) {
        return result;
    }

    QDomNodeList columns = node.childNodes();
    for(int i = 0; i < columns.size(); i++) {
        std::string columnNumStr = columns.at(i).nodeName().toStdString();
        QDomNode columnDataDomNode = FindChildDomNodeByName(columns.at(i), std::string("#text"));
        std::string columnDataString = columnDataDomNode.nodeValue().toStdString().c_str();
        int columnNum;
        float element[4];
        int ret = sscanf_s(columnNumStr.c_str(), "column%d", &columnNum);
        if (ret != 1) return glm::mat4(1.0f);
        ret = sscanf_s(columnDataString.c_str(), "%f %f %f %f", &element[0], &element[1], &element[2], &element[3]);
        if (ret != 4) return glm::mat4(1.0f);

        for (int r = 0; r < 4; r++) {
            result[columnNum][r] = element[r];
        }
    }

    return result;
}

glm::vec3 SceneReader::ReadGlmVec3FromDomNode(const QDomNode& node) {
    if (node.isNull()) {
        return glm::vec3(1.0f);
    }
    std::string type = FindAttributeByName(node, std::string("type"));   
    QDomNode dataDomNode = FindChildDomNodeByName(node, std::string("#text"));
    std::string dataStr = dataDomNode.nodeValue().toStdString();    
    if (type == std::string("vec3")) {
        return StdStringToGlmVec3(dataStr);
    }
    return glm::vec3(1.0f);
}

glm::vec2 SceneReader::ReadGlmVec2FromDomNode(const QDomNode& node) {
    if (node.isNull()) {
        return glm::vec2(1.0f);
    }
    std::string type = FindAttributeByName(node, std::string("type"));
    QDomNode dataDomNode = FindChildDomNodeByName(node, std::string("#text"));
    std::string dataStr = dataDomNode.nodeValue().toStdString();
    if (type == std::string("vec2")) {
        return StdStringToGlmVec2(dataStr);
    }
    return glm::vec2(1.0f);
}

std::string SceneReader::ReadStringFromDomNode(const QDomNode& node) {
    if (node.isNull()) {
        return std::string("");
    }
    QDomNode dataDomNode = FindChildDomNodeByName(node, std::string("#text"));
    if (dataDomNode.isNull()) {
        return std::string("");
    }
    return dataDomNode.nodeValue().toStdString();
}

float SceneReader::ReadFloatFromDomNode(const QDomNode& node) {
    if (node.isNull()) {
        return 0.0f;
    }
    std::string type = FindAttributeByName(node, std::string("type"));
    if (type != "float") {
        return 0.0f;
    }
    QDomNode dataDomNode = FindChildDomNodeByName(node, std::string("#text"));
    if (!dataDomNode.isNull()) {
        return std::stof(dataDomNode.nodeValue().toStdString());
    }
    return 0.0f;
}

int SceneReader::ReadIntFromDomNode(const QDomNode& node) {
    if (node.isNull()) {
        return 0;
    }
    std::string type = FindAttributeByName(node, std::string("type"));
    if (type != "int") {
        return 0;
    }
    QDomNode dataDomNode = FindChildDomNodeByName(node, std::string("#text"));
    if (!dataDomNode.isNull()) {
        return std::stoi(dataDomNode.nodeValue().toStdString());
    }
    return 0;
}

std::vector<Mesh*> SceneReader::LoadModel(std::string modelPath, float scale) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(modelPath, aiProcess_Triangulate);

    std::vector<Mesh*> result(0);

    // 检查
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cout << "ASSIMP IMPORT FAILED" << importer.GetErrorString() << std::endl;
        return result;
    }

    // 遍历所有mesh
    std::cout << "scene->mNumMeshes=" << scene->mNumMeshes << std::endl;
    for (int i = 0; i < scene->mNumMeshes; i++) {
        std::cout << "name = " << scene->mMeshes[i]->mName.C_Str() << std::endl;

        // 读取材质
        int matIndex = scene->mMeshes[i]->mMaterialIndex;
        std::string matName = scene->mMaterials[matIndex]->GetName().C_Str();
        std::cout << "mat name = " << matName << std::endl;
        if(mMaterialMap.find(matName) == mMaterialMap.end()) {
            std::cout << "not found material" << std::endl; 
            continue;
        }

        Material* mat = mMaterialMap[matName];
        Mesh* mesh = new Mesh(scene->mMeshes[i], mat, scale);
        result.push_back(mesh);
    }
    return result;
}

}