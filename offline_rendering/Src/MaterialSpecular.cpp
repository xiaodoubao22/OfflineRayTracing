#include "MaterialSpecular.h"

MaterialSpecular::MaterialSpecular() : Material(SPECULAR) {
	mSpecularRate = glm::vec3(1.0f);
	return;
}

MaterialSpecular::MaterialSpecular(glm::vec3 specularRate) : Material(SPECULAR) {
	mSpecularRate = specularRate;
	return;
}

bool MaterialSpecular::SampleAndEval(const glm::vec3& normal, const glm::vec3& wi, glm::vec3& wo, float& pdf, glm::vec3& fr) {
	float dotWiToNormal = dot(wi, normal);
	if (dotWiToNormal < 0.0f) {
		wo = normalize(glm::reflect(wi, normal));
		pdf = 1.0f;
		fr = mSpecularRate;
		return true;
	}
	return false;
}

bool MaterialSpecular::SampleWithImportance(const glm::vec3& normal, const glm::vec3& wi, glm::vec3& wo, float& pdf) {
	if (dot(wi, normal) < 0.0f) {
		wo = normalize(glm::reflect(wi, normal));
		pdf = 1.0f;
		return true;
	}
	return false;
}

glm::vec3 MaterialSpecular::Eval(const glm::vec3& normal, const glm::vec3& wi, const glm::vec3& wo) {
	if (dot(wi, normal) <= 0.0f && dot(wo, normal) >= 0.0f) {
		glm::vec3 halfVector = glm::normalize(-wi + wo);
		if (glm::length(halfVector - normal) < 0.1f) {
			return mSpecularRate;
		}
	}
	return glm::vec3(0.0f);
}