#include "BoundingBox.h"
#include "glm/common.hpp"

BoundingBox::BoundingBox() {
	mMin = glm::vec3(FLT_MAX);
	mMax = glm::vec3(-FLT_MAX);
}

BoundingBox::BoundingBox(glm::vec3 a, glm::vec3 b) {
	mMin = Utils::GlmVecMin(a, b);
	mMax = Utils::GlmVecMax(a, b);
}

BoundingBox::~BoundingBox() {

}

float BoundingBox::GetVolum() {
	glm::vec3 delta = mMax - mMin;
	return delta.x * delta.y * delta.z;
}

float BoundingBox::GetSurfaceArea() {
	glm::vec3 delta = mMax - mMin;
	return (delta.x * delta.y + delta.y * delta.z + delta.x * delta.z) * 2.0f;
}

int BoundingBox::GetMaxAxis() {
	glm::vec3 delta = mMax - mMin;
	float maxValue = delta.x;
	int ret = 0;
	if (delta.y > maxValue) {
		maxValue = delta.y;
		ret = 1;
	}
	if (delta.z > maxValue) {
		maxValue = delta.z;
		ret = 2;
	}

	return ret;
}

bool BoundingBox::Intersect(const Ray& ray) {
	glm::vec3 invDirection(1.0f / ray.direction);
	
	// P=O+t*D <=> t=(P-O)/D
	glm::vec3 tMin = (mMin - ray.origin) * invDirection;
	glm::vec3 tMax = (mMax - ray.origin) * invDirection;

	// 选择正确的出入口
	if (ray.direction.x < 0.0f) {
		std::swap(tMin.x, tMax.x);
	}
	if (ray.direction.y < 0.0f) {
		std::swap(tMin.y, tMax.y);
	}
	if (ray.direction.z < 0.0f) {
		std::swap(tMin.z, tMax.z);
	}

	float tEnter = std::max({ tMin.x, tMin.y, tMin.z });
	float tExit = std::min({ tMax.x, tMax.y, tMax.z });

	return (tExit - tEnter > -Consts::EPS && tExit >= -Consts::EPS);
}

void BoundingBox::BoundPoint(glm::vec3 p) {
	mMin = Utils::GlmVecMin(mMin, p);
	mMax = Utils::GlmVecMax(mMax, p);
}

void BoundingBox::Union(const BoundingBox& b1) {
	mMin = Utils::GlmVecMin(mMin, b1.mMin);
	mMax = Utils::GlmVecMax(mMax, b1.mMax);
}

BoundingBox BoundingBox::Union(const BoundingBox& b1, const BoundingBox& b2) {
	glm::vec3 min = Utils::GlmVecMin(b1.mMin, b2.mMin);
	glm::vec3 max = Utils::GlmVecMax(b1.mMax, b2.mMax);
	return BoundingBox(min, max);
}