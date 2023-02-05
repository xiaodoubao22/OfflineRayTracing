#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "Shape.h"
#include "Utils.h"

class Triangle : public Shape 
{
public:
	glm::vec3 mP0;
	glm::vec3 mP1;
	glm::vec3 mP2;
	glm::vec3 mNormal;
	float mArea;

public:
	Triangle();
	Triangle(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, Material* material);

	virtual HitResult Intersect(Ray ray) override;
	virtual void UniformSample(float& pdf, HitResult& res) override;
	virtual float GetArea() override;
	virtual glm::vec3 GetCenter() override;
};

#endif // !TRIANGLE_H
