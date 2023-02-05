#include "Sphere.h"

Sphere::Sphere() {

}

Sphere::Sphere(glm::vec3 O, float R, Material* material) {
	mCenter = O;
	mRadius = R;
	mRadiusSquare = R * R;
	mMaterial = material;

	mArea = 4.0f * Consts::M_PI * mRadiusSquare;

	glm::vec3 min = O - glm::vec3(R) - glm::vec3(Consts::EPS);
	glm::vec3 max = O + glm::vec3(R) + glm::vec3(Consts::EPS);
	mBound = new BoundingBox(min, max);
}

HitResult Sphere::Intersect(Ray ray) {
	HitResult res;

	glm::vec3 O = ray.origin;
	glm::vec3 D = ray.direction;

	glm::vec3 L = mCenter - O;
	float s = dot(L, D);
	float lSquare = dot(L, L);

	if (s < 0 && lSquare > mRadiusSquare - 0.0001) return res;

	float sSquare = s * s;
	float mSquare = lSquare - sSquare;
	if (mSquare > mRadiusSquare) return res;

	float q = std::sqrt(mRadiusSquare - mSquare);

	float t1 = s - q;
	float t2 = s + q;
	float t = t1;
	if (t1 < 0.0001) {
		t = t2;
	}

	glm::vec3 P = O + t * D;

	res.isHit = 1;
	res.distance = t;
	res.hitPoint = P;
	res.normal = glm::normalize(P - mCenter);
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


