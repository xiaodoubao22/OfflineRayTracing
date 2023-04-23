#ifndef SCENE_READER_H
#define SCENE_READER_H

#include <string>
#include <unordered_map>
#include <QDomDocument>
#include "Scene.h"
#include "TexureSampler.h"
#include "MaterialSet.h"

namespace Hmi
{
    
class SceneReader 
{
public:
    SceneReader();
    ~SceneReader();

    static SceneReader* GetInstance();
    Scene* ReadFromFloder(std::string floderName);
    
private:
    int ProcessSceneRoot(const std::string& floderName, const QDomNode &sceneDomNode);
    int ProcessMaterial(const std::string& floderName, const QDomNode &materialDomNode);
    int ProcessTexure(const std::string& floderName, const QDomNode &texureDomNode);
    int ProcessEnvMap(const std::string& floderName, const QDomNode &envMapDomNode);
    int ProcessShape(const std::string& floderName, const QDomNode &shapeDomNode);
    int ProcessCamera(const std::string& floderName, const QDomNode &cameraDomNode);
    int ProcessConfig(const std::string& floderName, const QDomNode &configDomNode);

    MaterialDefuse* ReadDiffuseMaterial(const QDomNode &materialDomNode);
    MaterialSpecular* ReadSpecularMaterial(const QDomNode &materialDomNode);
    MaterialTransparent* ReadTransparentMaterial(const QDomNode &materialDomNode);
    MaterialCookTorrance* ReadCookTorranceMaterial(const QDomNode &materialDomNode);
    MaterialFrostedGlass* ReadFrostedGlassMaterial(const QDomNode &materialDomNode);

    std::vector<Mesh*> ReadMeshFromDomNode(const QDomNode &shapeDomNode, std::string floderName);
    Triangle* ReadTriangleFromDomNode(const QDomNode &shapeDomNode);

private:
    QDomNode FindChildDomNodeByName(const QDomNode& node, const std::string& name);
    std::string FindAttributeByName(const QDomNode& node, const std::string& name);
    glm::vec3 StdStringToGlmVec3(const std::string& str);
    glm::vec2 StdStringToGlmVec2(const std::string& str);

    glm::mat4 ReadGlmMat4FromDomNode(const QDomNode& node);
    glm::vec3 ReadGlmVec3FromDomNode(const QDomNode& node);
    glm::vec2 ReadGlmVec2FromDomNode(const QDomNode& node);
    std::string ReadStringFromDomNode(const QDomNode& node);
    float ReadFloatFromDomNode(const QDomNode& node);
    int ReadIntFromDomNode(const QDomNode& node);

private:
    std::unordered_map<std::string, TexureSampler*> mTexureMap;
    std::unordered_map<std::string, TexureSampler2D<float>*> mTexureMapC1;
    std::unordered_map<std::string, Material*> mMaterialMap;
    std::vector<Shape*> mShapes;
    std::pair<bool, CpuEngin::Camera> mCamera = {false, CpuEngin::Camera()};
    std::pair<bool, CpuEnginConfig> mConfigInfo = {false, CpuEnginConfig()};
    std::pair<std::string, SphericalMap*> mEnvMap = {"", nullptr};

private:
    std::unordered_map<std::string, int> mTexureTypeMap = {
        {"roughness", 1}, 
        {"luminance", 1}, 
        {"r", 1}, 
        {"rg", 2}, 
        {"rgb", 3},
        {"rgba", 4}
    };
};

}


#endif // !SCENE_READER_H