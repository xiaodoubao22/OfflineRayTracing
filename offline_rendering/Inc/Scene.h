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
	std::vector<Shape*> mShapes;
	std::vector<Material*> mMaterials;
	std::vector<TexureSampler*> mTexures;

public:
	Scene();
	~Scene();
	bool PushShape(Shape* shape);
	bool PushMaterial(Material* material);
	bool PushTexure(TexureSampler* texure);
	HitResult GetIntersect(Ray ray);
	bool SampleLight(float& pdf, HitResult& res);

	static std::vector<Mesh*> LoadModel(std::string modelPath, Material* material);

};

#endif // !SCENE_H

