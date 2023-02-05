#include "Material.h"

Material::Material(MaterialType type) {
	mType = type;
}

MaterialType Material::GetType() {
	return mType;
}

bool Material::HasEmission() {
	return mHasEmission;
}

glm::vec3 Material::GetEmission() {
	return mEmissionRadiance;
}

float Material::Fresnel(glm::vec3 wi, glm::vec3 normal, float ior) {
	float dotWiTonormalormal = dot(wi, normal);
	float etaI = 1.0f, etaT = ior;		// dotWiTonormalormal < 0
	if (dotWiTonormalormal > 0) {
		std::swap(etaI, etaT);			// dotWiTonormalormal > 0
	}

	float sinI = sqrtf(std::max(0.0f, 1 - dotWiTonormalormal * dotWiTonormalormal));
	float sinT = etaI / etaT * sinI;

	float kr = 1.0f;
	if (sinT < 1.0f) {
		float cosT = sqrtf(std::max(0.f, 1 - sinT * sinT));
		float cosI = abs(dotWiTonormalormal);
		float Rs = ((etaT * cosI) - (etaI * cosT)) / ((etaT * cosI) + (etaI * cosT));
		float Rp = ((etaI * cosI) - (etaT * cosT)) / ((etaI * cosI) + (etaT * cosT));
		kr = (Rs * Rs + Rp * Rp) / 2;
	}
	return kr;
}

glm::vec3 Material::LocalToWorld(glm::vec3 vec, glm::vec3 normal) {
	glm::vec3 axisLocX = cross(glm::vec3(0.0f, 0.0f, 1.0f), normal);
	glm::vec3 axisLocY = cross(normal, axisLocX);
	glm::mat3 rot = glm::mat3(glm::lookAtRH(glm::vec3(0.0f), -normal, axisLocY));

	return glm::transpose(rot) * vec;
}
