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
	float dotWiTonormal = std::clamp(dot(wi, normal), -1.0f, 1.0f);
	float etaI = 1.0f, etaT = ior;		// dotWiTonormal < 0
	if (dotWiTonormal > 0) {
		std::swap(etaI, etaT);			// dotWiTonormal > 0
	}

	float sinI = std::sqrt(std::max(0.0f, 1.0f - dotWiTonormal * dotWiTonormal));
	float sinT = etaI / etaT * sinI;

	float kr = 1.0f;
	if (sinT < 1.0f) {
		float cosT = std::sqrt(std::max(0.f, 1 - sinT * sinT));
		float cosI = abs(dotWiTonormal);
		float Rs = ((etaT * cosI) - (etaI * cosT)) / ((etaT * cosI) + (etaI * cosT));
		float Rp = ((etaI * cosI) - (etaT * cosT)) / ((etaI * cosI) + (etaT * cosT));
		kr = (Rs * Rs + Rp * Rp) / 2.0f;
	}
	return kr;
}

glm::vec3 Material::LocalToWorld(glm::vec3 vec, glm::vec3 normal) {
	float dotZToNormal = dot(Consts::Z_AXIS, normal);
	if (dotZToNormal > 1.0 - Consts::EPS) return vec;
	if (dotZToNormal < -1.0 + Consts::EPS) return -vec;

	float rotAngle = acos(std::clamp(dotZToNormal, -1.0f, 1.0f));
	glm::vec3 rotAxis = glm::cross(Consts::Z_AXIS, normal);
	glm::mat4 model = glm::rotate(glm::mat4(1.0f), rotAngle, rotAxis);

	glm::mat3 modelRot = glm::mat3(model);
	return modelRot * vec;
}
