#include "Triangle.h"

#include <iostream>

Triangle::Triangle() {

}

Triangle::Triangle(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, Material* material) : mP0(p0), mP1(p1), mP2(p2) {
	glm::vec3 e1 = p1 - p0;
	glm::vec3 e2 = p2 - p0;
	glm::vec3 crossE1ToE2 = cross(e1, e2);

	mNormal = normalize(crossE1ToE2);
	mMaterial = material;
	mArea = glm::length(crossE1ToE2) * 0.5f;

	glm::vec3 min = Utils::GlmVecMin(p0, p1, p2) - glm::vec3(Consts::EPS);
	glm::vec3 max = Utils::GlmVecMax(p0, p1, p2) + glm::vec3(Consts::EPS);

	mBound = new BoundingBox(min, max);
}

HitResult Triangle::Intersect(Ray ray) {
	HitResult res;
	
	glm::vec3 E1 = mP1 - mP0;
	glm::vec3 E2 = mP2 - mP0;
	glm::vec3 Q = cross(ray.direction, E2);

	float a = dot(E1, Q);
	if (abs(a) < 0.00001f) return res;

	float f = 1.0f / a;
	glm::vec3 S = ray.origin - mP0;
	float u = f * dot(S, Q);
	if (u < 0.0) return res;

	glm::vec3 R = cross(S, E1);
	float v = f * dot(ray.direction, R);
	if (v < 0.0f || u + v > 1.0f) return res;

	float t = f * dot(E2, R);
	if (t < 0.0001f) return res;

	res.isHit = 1;
	res.hitPoint = ray.origin + t * ray.direction;
	res.distance = t;
	res.material = mMaterial;
	res.normal = mNormal;

	return res;
}


void Triangle::UniformSample(float& pdf, HitResult& res) {
	float sqrtRand1 = std::sqrt(Utils::GetUniformRandom());
	float rand2 = Utils::GetUniformRandom();

	float a0 = 1.0f - sqrtRand1;
	float a1 = sqrtRand1 * (1.0f - rand2);
	float a2 = sqrtRand1 * rand2;

	res.hitPoint = a0 * mP0 + a1 * mP1 + a2 * mP2;
	res.normal = mNormal;
	res.material = mMaterial;
	res.isHit = 1;
	pdf = 1.0f / mArea;
}

float Triangle::GetArea() {
	return mArea;
}

glm::vec3 Triangle::GetCenter() {
	return (mP0 + mP1 + mP2) / 3.0f;
}

