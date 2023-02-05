#ifndef SPHERE_H
#define SPHERE_H

#include "Shape.h"
#include "Utils.h"

class Sphere : public Shape
{
public:
	glm::vec3 mCenter;
	float mRadius;
	float mRadiusSquare;
	float mArea;

public:
	Sphere();
	Sphere(glm::vec3 O, float R, Material* material);
	virtual HitResult Intersect(Ray ray);
	virtual void UniformSample(float& pdf, HitResult& res);
	virtual float GetArea();
	virtual glm::vec3 GetCenter();
};

#endif // !SPHERE_H
