#include "MaterialCookTorrance.h"

MaterialCookTorrance::MaterialCookTorrance() : Material(COOK_TORRANCE) {
	mF0 = glm::vec3(1.0f);
	mRoughness = 0.5f;
	mAlpha = mRoughness * mRoughness;
	mAlphaSquare = mAlpha * mAlpha;
	mK = pow(mRoughness + 1.0f, 2) / 8.0f;
}

MaterialCookTorrance::MaterialCookTorrance(float roughness, glm::vec3 f0) : Material(COOK_TORRANCE) {
	mF0 = f0;
	mRoughness = roughness;
	mAlpha = mRoughness * mRoughness;
	mAlphaSquare = mAlpha * mAlpha;
	mK = pow(mRoughness + 1.0f, 2) / 8.0f;
}

bool MaterialCookTorrance::SampleAndEval(const glm::vec3& normal, const glm::vec3& wi, glm::vec3& wo, float& pdf, glm::vec3& fr) {
	float dotWiToNormal = dot(wi, normal);
	if (dotWiToNormal < 0.0f) {
		// sample wo
		float phi = Utils::GetUniformRandom(0, 2 * Consts::M_PI);
		float ksi = Utils::GetUniformRandom();
		float cosThetaSquare = (1.0f - ksi) / (ksi * (mAlphaSquare - 1.0f) + 1.0f);
		float cosTheta = std::sqrt(cosThetaSquare);
		float sinTheta = std::sqrt(1.0f - cosThetaSquare);
		glm::vec3 localWh(sinTheta * std::cos(phi), sinTheta * std::sin(phi), cosTheta);
		glm::vec3 wh = LocalToWorld(localWh, normal);
		wo = glm::reflect(wi, wh);

		// fr
		float dotWoToNormal = dot(wo, normal);
		glm::vec3 halfVector = normalize(-wi + wo);
		float Distribution = DistributionGGX(normal, halfVector);
		glm::vec3 Fresnel = FresnelSchlic(abs(dotWiToNormal));
		float Geometry = GeometrySmith(-dotWiToNormal, dotWoToNormal);
		fr = Distribution * Fresnel * Geometry / (4.0f * (-dotWiToNormal));	// BRDF * cosine

		// pdf
		pdf = Distribution * dot(wh, normal);

		return true;
	}
	return false;
}

bool MaterialCookTorrance::SampleWithImportance(const glm::vec3& normal, const glm::vec3& wi, glm::vec3& wo, float& pdf) {
	if (dot(wi, normal) < 0.0f) {
		float phi = Utils::GetUniformRandom(0, 2 * Consts::M_PI);
		float ksi = Utils::GetUniformRandom();
		float cosThetaSquare = (1.0f - ksi) / (ksi * (mAlphaSquare - 1.0f) + 1.0f);
		float cosTheta = std::sqrt(cosThetaSquare);
		float sinTheta = std::sqrt(1.0f - cosThetaSquare);
		glm::vec3 localWh(sinTheta * std::cos(phi), sinTheta * std::sin(phi), cosTheta);
		glm::vec3 wh = LocalToWorld(localWh, normal);
		wo = glm::reflect(wi, wh);

		float dotWoToNormal = dot(wo, normal);
		glm::vec3 halfVector = normalize(-wi + wo);
		float Distribution = DistributionGGX(normal, halfVector);
		pdf = Distribution * dot(wh, normal);

		return true;
	}
	return false;
}

glm::vec3 MaterialCookTorrance::Eval(const glm::vec3& normal, const glm::vec3& wi, const glm::vec3& wo) {
	float dotWiToNormal = dot(wi, normal);
	float dotWoToNormal = dot(wo, normal);
	if (dotWiToNormal < 0.0f && dotWoToNormal > 0.0f) {
		glm::vec3 halfVector = normalize(-wi + wo);
		float Distribution = DistributionGGX(normal, halfVector);
		glm::vec3 Fresnel = FresnelSchlic(abs(dotWiToNormal));
		float Geometry = GeometrySmith(-dotWiToNormal, dotWoToNormal);

		return Distribution * Fresnel * Geometry / (4.0f * (-dotWiToNormal));	// BRDF * cosine
	}
	return glm::vec3(0.0f);
}

float MaterialCookTorrance::DistributionGGX(glm::vec3 normal, glm::vec3 wh) {
	float dotNormalToWh = dot(normal, wh);
	float denom = Consts::M_PI * powf(dotNormalToWh * dotNormalToWh * (mAlphaSquare - 1.0f) + 1, 2.0f);
	return mAlphaSquare / denom;
}

float MaterialCookTorrance::GeometrySchlickGGX(float dotNormalToW) {
	return dotNormalToW / (dotNormalToW * (1.0f - mK) + mK);
}

float MaterialCookTorrance::GeometrySmith(float absDotWiToNormal, float absDotWoToNormal) {
	float G1 = GeometrySchlickGGX(absDotWiToNormal);
	float G2 = GeometrySchlickGGX(absDotWoToNormal);
	return G1 * G2;
}

glm::vec3 MaterialCookTorrance::FresnelSchlic(float absDotWiToNormal) {
	float cosTheta = std::max(absDotWiToNormal, 1.0f);
	return mF0 + (glm::vec3(1.0f) - mF0) * powf(1.0f - cosTheta, 5.0f);
}