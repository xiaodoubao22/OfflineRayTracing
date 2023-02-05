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

	glm::vec3 D = ray.direction;
	glm::vec3 S = ray.origin;
	glm::vec3 N = mNormal;

	float dotTtoD = dot(N, D);
	if (dotTtoD > 0.0f) {
		// 保证法向量与光线方向相反
		N = -N;
	}

	// 光线与三角形平行
	if (fabs(dotTtoD) < Consts::EPS) {
		res.isHit = 2;
		return res;
	}

	float t = (dot(N, mP0) - dot(S, N)) / dot(D, N);

	// 交点在射线反方向
	if (t < 0.01) {
		res.isHit = 3;
		return res;
	}

	glm::vec3 P = S + t * D;

	// 判断交点在三角形内
	glm::vec3 c1 = cross(mP1 - mP0, P - mP0);
	glm::vec3 c2 = cross(mP2 - mP1, P - mP1);
	glm::vec3 c3 = cross(mP0 - mP2, P - mP2);

	if (dot(c1, mNormal) < 0 || dot(c2, mNormal) < 0 || dot(c3, mNormal) < 0) return res;

	// 返回结果
	res.isHit = 1;
	res.hitPoint = P;
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

