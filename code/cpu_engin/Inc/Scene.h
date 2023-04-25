#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include "Shape.h"
#include "Mesh.h"
#include "Utils.h"
#include "TexureSampler.h"
#include "Camera.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

struct CpuEnginConfig {
	uint32_t spp = 16;
	uint32_t threadCount = 8;
	uint32_t maxTraceDepth = 10;
};

class Scene {
public:
	int mNumIlluminant = 0;
	std::vector<Shape*> mShapes;
	std::vector<Material*> mMaterials;
	std::vector<TexureSampler*> mTexures;
	SphericalMap* mLightMap = nullptr;
	CpuEngin::Camera mCamera;
	CpuEnginConfig mConfigInfo;

public:
	Scene();
	~Scene();
	bool PushShape(Shape* shape);
	bool PushMaterial(Material* material);
	bool PushTexure(TexureSampler* texure);
	bool PushmLightMap(SphericalMap* lightMap);
	HitResult GetIntersect(Ray ray);
	bool SampleLight(float& pdf, HitResult& res);

	// static std::unordered_map<std::string, Mesh*> Scene::LoadModel(std::string modelPath, float scale=1.0f);

};

#endif // !SCENE_H
