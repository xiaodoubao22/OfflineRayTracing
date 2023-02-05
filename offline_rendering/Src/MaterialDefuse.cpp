#include "MaterialDefuse.h"

MaterialDefuse::MaterialDefuse() : Material(DEFUSE) {
	return;
}

MaterialDefuse::MaterialDefuse(glm::vec3 albedo) : Material(DEFUSE) {
	mAlbedo = albedo;
	return;
}

bool MaterialDefuse::SampleAndEval(const glm::vec3& normal, const glm::vec3& wi, glm::vec3& wo, float& pdf, glm::vec3& fr) {
	if (dot(wi, normal) < 0.0f) {
		wo = Utils::randomDirection(normal);
		pdf = 1.0f / (2.0f * Consts::M_PI);
		float cosine = std::max(dot(wo, normal), 0.0f);
		fr = mAlbedo / Consts::M_PI * cosine;
		return true;
	}

	return false;

}

bool MaterialDefuse::SampleWithImportance(const glm::vec3& normal, const glm::vec3& wi, glm::vec3& wo, float& pdf) {
	if (dot(wi, normal) < 0.0f) {
		wo = Utils::randomDirection(normal);
		pdf = 1.0f / (2.0f * Consts::M_PI);
		return true;
	}
	return false;
}

glm::vec3 MaterialDefuse::Eval(const glm::vec3& normal, const glm::vec3& wi, const glm::vec3& wo) {
	float dotWiToNormal = dot(wi, normal);
	float dotWoToNormal = dot(wo, normal);
	if (dotWiToNormal < 0.0f && dotWoToNormal > 0.0f) {
		return mAlbedo / Consts::M_PI * dotWoToNormal;
	}
	return glm::vec3(0.0f);
}
