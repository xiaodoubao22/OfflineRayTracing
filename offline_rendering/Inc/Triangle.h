#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "Shape.h"
#include "Utils.h"

class Triangle : public Shape 
{
public:
	glm::vec3 mP[3];
	glm::vec3 mN[3];
	glm::vec2 mT[3];
	glm::vec3 mNormal;
	float mArea;
	bool mHasNormals = false;
	bool mHasTexCoords = false;

public:
	Triangle();
	Triangle(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, Material* material);
	Triangle(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, 
		     glm::vec3 n0, glm::vec3 n1, glm::vec3 n2, Material* material);
	Triangle(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2,
		     glm::vec3 n0, glm::vec3 n1, glm::vec3 n2, 
		     glm::vec2 t0, glm::vec2 t1, glm::vec2 t2, Material* material);

	virtual HitResult Intersect(Ray ray) override;
	virtual void UniformSample(float& pdf, HitResult& res) override;
	virtual float GetArea() override;
	virtual glm::vec3 GetCenter() override;
};

#endif // !TRIANGLE_H
