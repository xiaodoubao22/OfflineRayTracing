#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include "Shape.h"
#include "Mesh.h"
#include "Utils.h"
#include "TexureSampler.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class Scene {
public:
	int mNumIlluminant = 0;
	std::vector<Shape*> mShapes;
	std::vector<Material*> mMaterials;
	std::vector<TexureSampler*> mTexures;
	SphericalMap* mLightMap = nullptr;
	glm::mat4 mViewmatrix = glm::mat4(1.0f);


public:
	Scene();
	~Scene();
	bool PushShape(Shape* shape);
	bool PushMaterial(Material* material);
	bool PushTexure(TexureSampler* texure);
	bool PushmLightMap(SphericalMap* lightMap);
	HitResult GetIntersect(Ray ray);
	bool SampleLight(float& pdf, HitResult& res);

	static std::vector<Mesh*> LoadModel(std::string modelPath, Material* material, float scale = 1.0f);

};

#endif // !SCENE_H
