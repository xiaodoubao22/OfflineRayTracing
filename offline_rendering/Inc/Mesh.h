#ifndef MESH_H
#define MESH_H

#include "Shape.h"
#include "Triangle.h"
#include "BvhTree.h"
#include "assimp/scene.h"

class Mesh : public Shape
{
public:
	Mesh();
	explicit Mesh(aiMesh* assImpMesh, Material* material);
	~Mesh();
	virtual HitResult Intersect(Ray ray) override;
	virtual void UniformSample(float& pdf, HitResult& res) override;
	virtual float GetArea() override;
	virtual glm::vec3 GetCenter() override;

public:
	std::vector<Shape*> mTriangles;
	BvhTree mBvhTree;

	glm::mat4 mModelMatrix;
};


#endif // !MESH_H
