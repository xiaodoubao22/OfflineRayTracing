#include "MaterialTransparent.h"

MaterialTransparent::MaterialTransparent() : Material(TRANSPARENT) {
	mIor = 1.0f;
	return;
}

MaterialTransparent::MaterialTransparent(float ior) : Material(TRANSPARENT) {
	mIor = ior;
	return;
}

bool MaterialTransparent::SampleAndEval(const glm::vec3& normal, const glm::vec3& wi, glm::vec3& wo, float& pdf, glm::vec3& fr) {
	float kr = Fresnel(wi, normal, mIor);
	float rand = Utils::GetUniformRandom();
	if (rand < kr) {
		// reflect
		wo = glm::reflect(wi, normal);
		pdf = 1.0f;
		fr = glm::vec3(1.0f);
	}
	else {
		// refract
		if (dot(wi, normal) < 0.0f) {
			float eta = 1.0f / mIor;
			wo = glm::refract(wi, normal, eta);
		}
		else {
			float eta = mIor;
			wo = glm::refract(wi, -normal, eta);
		}
		wo = glm::normalize(wo);
		pdf = 1.0f;
		fr = glm::vec3(1.0f);
	}
	return true;
}

bool MaterialTransparent::SampleWithImportance(const glm::vec3& normal, const glm::vec3& wi, glm::vec3& wo, float& pdf) {
	float kr = Fresnel(wi, normal, mIor);
	float rand = Utils::GetUniformRandom();
	if (rand < kr) {
		// reflect
		wo = glm::reflect(wi, normal);
		pdf = 1.0f;
	}
	else {
		// refract
		wo = glm::refract(wi, normal, 1.0f / mIor);
		pdf = 1.0f;
	}
	return true;
}

glm::vec3 MaterialTransparent::Eval(const glm::vec3& normal, const glm::vec3& wi, const glm::vec3& wo) {
	float dotNtoWi = dot(normal, wi);
	float dotNtoWo = dot(normal, wo);
	if (dotNtoWi * dotNtoWo > 0) {
		// refract
		glm::vec3 ht_N;
		if (dotNtoWi > 0) {
			ht_N = normalize(mIor * wi - wo);
		}
		else {
			ht_N = normalize(wi - mIor * wo);
		}

		if (glm::length(ht_N - normal) < 0.5f) {
			//float kr = Fresnel(wi, normal, mIor);
			return glm::vec3(1.0f);
		}
		
	}
	else {
		// reflect
		glm::vec3 halfVector = glm::normalize(-wi + wo);
		if (glm::length(halfVector - normal) < 0.1f) {
			//float kr = Fresnel(wi, normal, mIor);
			return glm::vec3(1.0f);
		}
	}

	return glm::vec3(0.0f);
}