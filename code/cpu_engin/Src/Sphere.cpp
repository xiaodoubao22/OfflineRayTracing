#include "Sphere.h"

Sphere::Sphere() {

}

Sphere::Sphere(glm::vec3 O, float R, Material* material) {
	mCenter = O;
	mRadius = R;
	mRadiusSquare = R * R;
	mMaterial = material;

	mArea = 4.0f * Consts::PI * mRadiusSquare;

	glm::vec3 min = O - glm::vec3(R) - glm::vec3(Consts::EPS);
	glm::vec3 max = O + glm::vec3(R) + glm::vec3(Consts::EPS);
	mBound = new BoundingBox(min, max);
}

HitResult Sphere::Intersect(Ray ray) {
	HitResult res;

	// 光线转到局部坐标系下
	ray.origin = mModel * glm::vec4(ray.origin, 1.0f);
	ray.direction = glm::mat3(mModel) * ray.direction;

	glm::vec3 O = ray.origin;
	glm::vec3 D = ray.direction;

	glm::vec3 L = mCenter - O;
	float s = dot(L, D);
	float lSquare = dot(L, L);

	if (s < 0 && lSquare > mRadiusSquare) return res;

	float sSquare = s * s;
	float mSquare = lSquare - sSquare;
	if (mSquare > mRadiusSquare) return res;

	float q = std::sqrt(mRadiusSquare - mSquare);

	float t1 = s - q;
	float t2 = s + q;
	float t = t1;
	if (t1 < 0.0001f * Consts::SCALE) {
		t = t2;
	}
	if (t < 0.0001f * Consts::SCALE) {
		return res;
	}

	glm::vec3 P = O + t * D;

	//if (dot(glm::normalize(P - mCenter), ray.direction) > 0) return res;

	res.isHit = 1;
	res.distance = t;
	res.hitPoint = mModelInv * glm::vec4(P, 1.0f);
	res.normal = glm::mat3(mModelInv) * glm::normalize(P - mCenter);
	res.material = mMaterial;

	return res;
}

void Sphere::UniformSample(float& pdf, HitResult& res) {
	std::cout << "error" << std::endl;

}

float Sphere::GetArea() {
	return mArea;
}

glm::vec3 Sphere::GetCenter() {
	return mCenter;
}


