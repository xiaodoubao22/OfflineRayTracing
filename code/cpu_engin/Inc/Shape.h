#ifndef SHAPE_H
#define SHAPE_H

#include "Utils.h"
#include "HitResult.h"
#include "BoundingBox.h"

class Shape {
public:
	Material* mMaterial = nullptr;
	BoundingBox* mBound = nullptr;
	glm::mat4 mModel;
	glm::mat4 mModelInv;

public:
	Shape();
	virtual ~Shape();

	virtual HitResult Intersect(Ray ray) = 0;
	virtual void UniformSample(float& pdf, HitResult& res) = 0;
	virtual float GetArea() = 0;
	virtual glm::vec3 GetCenter() = 0;
	bool HasEmission();
	BoundingBox* GetBoundingBox();
	void SetModelMatrix(const glm::mat4& model);
	
};



#endif // SHAPE_H