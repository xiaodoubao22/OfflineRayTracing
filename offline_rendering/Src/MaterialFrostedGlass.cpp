#include "MaterialFrostedGlass.h"

MaterialFrostedGlass::MaterialFrostedGlass() : Material(FROSTED_GLASS) {
	mF0 = glm::vec3(1.0f);
	mRoughness = 0.5f;
	mAlpha = mRoughness * mRoughness;
	mAlphaSquare = mAlpha * mAlpha;
	mK = pow(mAlpha + 1.0f, 2) / 8.0f;
}

MaterialFrostedGlass::MaterialFrostedGlass(float roughness, float ior, glm::vec3 f0) : Material(FROSTED_GLASS) {
	mIor = ior;
	mF0 = f0;
	mRoughness = roughness;
	mAlpha = mRoughness * mRoughness;
	mAlphaSquare = mAlpha * mAlpha;
	mK = pow(0.0f + 1.0f, 2) / 8.0f;
}

bool MaterialFrostedGlass::SampleAndEval(const glm::vec3& normal, const glm::vec3& wi, glm::vec3& wo, float& pdf, glm::vec3& fr) {
	// sample wh
	float phi = Utils::GetUniformRandom(0, 2 * Consts::M_PI);
	float ksi = Utils::GetUniformRandom();
	float cosThetaSquare = (1.0f - ksi) / (ksi * (mAlphaSquare - 1.0f) + 1.0f);
	float cosTheta = std::sqrt(cosThetaSquare);
	float sinTheta = std::sqrt(1.0f - cosThetaSquare);
	glm::vec3 localWh(sinTheta * std::cos(phi), sinTheta * std::sin(phi), cosTheta);
	glm::vec3 wh = LocalToWorld(localWh, normal);
	//float distribution = DistributionGGX(normal, wh);

	// calculate wo and pdf
	float dotWiToNormal = dot(wi, normal);
	// choose reflect or refract according to fresnel term
	float fresnel = Material::Fresnel(wi, wh, mIor);
	float rand = Utils::GetUniformRandom();
	if (rand < fresnel) {
		// reflect
		// wo
		wo = glm::reflect(wi, wh);
		float dotWoToNormal = dot(wo, normal);
		if (dotWiToNormal * dotWoToNormal > 0.0f) {
			pdf = 0.0f;
			return false;
		}

		// pdf
		pdf = /*fresnel * distribution * */dot(wh, normal) / (4.0f * abs(dot(wi, wh)));

		// fr
		float geometry = GeometrySmith(abs(dotWiToNormal), abs(dotWoToNormal));
		float frFactor = /*fresnel * distribution * */geometry / (4.0f * (-dotWiToNormal));	// BRDF * cosine
		fr = glm::vec3(frFactor);
	}
	else {
		// refract
		// wo
		if (dot(wi, wh) < 0.0f) {
			float eta = 1.0f / mIor;
			wo = glm::refract(wi, wh, eta);
		}
		else {
			float eta = mIor;
			wo = glm::refract(wi, -wh, eta);
		}
		wo = glm::normalize(wo);
		//wo = glm::refract(wi, wh, 1.0f / mIor);
		float dotWoToNormal = dot(wo, normal);
		if (dotWiToNormal * dotWoToNormal < 0.0f) {
			return false;
		}

		// fr
		float etaI = 1.0f, etaT = mIor;
		if (dotWiToNormal > 0) {
			std::swap(etaI, etaT);
		}
		float dotWiToHt = dot(wi, wh);
		float dotWoToHt = dot(wo, wh);
		
		float geometry = GeometrySmith(abs(dotWiToNormal), abs(dotWoToNormal));

		float factor = abs(dotWiToHt) * abs(dotWoToHt) / (abs(dotWiToNormal)/* * abs(dotWoToNormal)*/); // factor * cosine
		float denom = -etaI * dotWiToHt + etaT * dotWoToHt;
		denom = denom * denom;
		float nom = etaI * etaI * /*(1.0f - fresnel) * distribution * */geometry;

		fr = glm::vec3(factor * nom / denom);
		//fr = glm::vec3(1.0f);

		// pdf
		pdf = /*(1.0f - fresnel) * distribution * */dot(wh, normal) / denom;
		//pdf = 1.0f;
	}

	return true;
}

bool MaterialFrostedGlass::SampleWithImportance(const glm::vec3& normal, const glm::vec3& wi, glm::vec3& wo, float& pdf) {
	// sample wh
	float phi = Utils::GetUniformRandom(0, 2 * Consts::M_PI);
	float ksi = Utils::GetUniformRandom();
	float cosThetaSquare = (1.0f - ksi) / (ksi * (mAlphaSquare - 1.0f) + 1.0f);
	float cosTheta = std::sqrt(cosThetaSquare);
	float sinTheta = std::sqrt(1.0f - cosThetaSquare);
	glm::vec3 localWh(sinTheta * std::cos(phi), sinTheta * std::sin(phi), cosTheta);
	glm::vec3 wh = LocalToWorld(localWh, normal);
	float Distribution = DistributionGGX(normal, wh);

	// calculate wo and pdf
	float dotWiToNormal = dot(wi, normal);
	// choose reflect or refract according to fresnel term
	float fresnel = Material::Fresnel(wi, wh, mIor);
	float rand = Utils::GetUniformRandom();
	if (rand < fresnel) {
		// reflect
		wo = glm::reflect(wi, wh);
		float dotWoToNormal = dot(wo, normal);
		if (dotWiToNormal * dotWoToNormal > 0.0f) {
			pdf = 0.0f;
			return false;
		}
		pdf = Distribution * dot(wh, normal) * fresnel;
	}
	else {
		// refract
		wo = glm::refract(wi, wh, 1.0f / mIor);
		float dotWoToNormal = dot(wo, normal);
		if (dotWiToNormal * dotWoToNormal < 0.0f) {
			pdf = 0.0f;
			return false;
		}
		pdf = Distribution * dot(wh, normal) * (1.0f - fresnel);
	}

	return true;
}

glm::vec3 MaterialFrostedGlass::Eval(const glm::vec3& normal, const glm::vec3& wi, const glm::vec3& wo) {
	float dotWiToNormal = dot(wi, normal);
	float dotWoToNormal = dot(wo, normal);
	if (dotWiToNormal * dotWoToNormal > 0 ) {
		// refract
		float etaI = 1.0f, etaT = mIor;
		if (dotWiToNormal > 0) {
			std::swap(etaI, etaT);
		}

		// ht
		glm::vec3 ht_N = normalize(- etaI * wi - etaT * wo);

		// fr
		float dotWiToHt = dot(wi, ht_N);
		float dotWoToHt = dot(wo, ht_N);

		float distribution = DistributionGGX(normal, ht_N);
		float fresnel = Material::Fresnel(wi, ht_N, mIor);
		float geometry = GeometrySmith(abs(dotWiToNormal), abs(dotWoToNormal));

		float factor = abs(dotWiToHt) * abs(dotWoToHt) / (abs(dotWiToNormal)/* * abs(dotWoToNormal)*/);	// factor * cosine
		float denom = -etaI * dotWiToHt + etaT * dotWoToHt;
		denom = denom * denom;
		float nom = etaI * etaI * (1.0f - fresnel) * distribution * geometry;

		return glm::vec3(factor * nom / denom);
		//return glm::vec3(1.0f);
	}
	else {
		// reflect
		glm::vec3 halfVector = normalize(-wi + wo);
		float Distribution = DistributionGGX(normal, halfVector);
		float fresnel = Material::Fresnel(wi, halfVector, mIor);
		float Geometry = GeometrySmith(abs(dotWiToNormal), abs(dotWoToNormal));

		return Distribution * glm::vec3(fresnel) * Geometry / (4.0f * (-dotWiToNormal));	// BRDF * cosine
	}
}

float MaterialFrostedGlass::DistributionGGX(glm::vec3 normal, glm::vec3 wh) {
	float dotNormalToWh = dot(normal, wh);
	float denom = Consts::M_PI * powf(dotNormalToWh * dotNormalToWh * (mAlphaSquare - 1.0f) + 1, 2.0f);
	return mAlphaSquare / denom;
}

float MaterialFrostedGlass::GeometrySchlickGGX(float dotNormalToW) {
	float cosThetaSquare = dotNormalToW * dotNormalToW;
	float sinThetaSquare = 1.0f - cosThetaSquare;
	float tanThetaSquare = sinThetaSquare / cosThetaSquare;
	float lamda = (sqrt(1.0f + mAlphaSquare * tanThetaSquare) - 1.0f) / 2.0f;
	return 1.0f / (1.0f + lamda);
}

float MaterialFrostedGlass::GeometrySmith(float absDotWiToNormal, float absDotWoToNormal) {
	float G1 = GeometrySchlickGGX(absDotWiToNormal);
	float G2 = GeometrySchlickGGX(absDotWoToNormal);
	return G1 * G2;
}

glm::vec3 MaterialFrostedGlass::FresnelSchlic(float absDotWiToNormal) {
	float cosTheta = std::max(absDotWiToNormal, 1.0f);
	return mF0 + (glm::vec3(1.0f) - mF0) * powf(1.0f - cosTheta, 5.0f);
}