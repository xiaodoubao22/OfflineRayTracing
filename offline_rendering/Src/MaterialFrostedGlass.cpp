#include "MaterialFrostedGlass.h"

MaterialFrostedGlass::MaterialFrostedGlass() : Material(FROSTED_GLASS) {
	mF0 = 0.04f;
	mRoughness = 0.5f;
	mAlpha = mRoughness * mRoughness;
	mAlphaSquare = mAlpha * mAlpha;
}

MaterialFrostedGlass::MaterialFrostedGlass(float roughness, float ior) : Material(FROSTED_GLASS) {
	mIor = ior;
	mF0 = pow((1.0f - ior) / (1.0f + ior), 2);
	mRoughness = roughness;
	mAlpha = mRoughness * mRoughness;
	mAlphaSquare = mAlpha * mAlpha;
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
	//float fresnel = Material::Fresnel(wi, wh, mIor);
	float fresnel = FresnelSchlic(wi, wh);
	float rand = Utils::GetUniformRandom();
	if (rand < fresnel) {
		// reflect
		// wo
		glm::vec3 H = wh;	// 指向入射侧，专供refract使用
		if (dotWiToNormal > 0) {
			H = -H;
		}

		wo = glm::normalize(glm::reflect(wi, H));
		float dotWoToNormal = dot(wo, normal);
		if (dotWiToNormal * dotWoToNormal > 0.0f) {
			pdf = 0.0f;
			return false;
		}
		float dotWiToH = dot(wi, wh);
		float dotWoToH = dot(wo, wh);

		// fr
		float geometry = GeometrySmith(abs(dotWiToH), abs(dotWoToH));

		float frFactor = /*fresnel * distribution * */geometry / (4.0f * abs(dotWiToNormal) + Consts::EPS);	// BRDF * cosine
		fr = glm::vec3(frFactor);

		// pdf
		pdf = /*fresnel * distribution * */dot(wh, normal) / (4.0f * abs(dotWiToH) + Consts::EPS);


	}
	else {
		// refract
		float etaI = 1.0f, etaT = mIor;
		glm::vec3 H = wh;	// 指向入射侧，专供refract使用
		if (dotWiToNormal > 0) {
			std::swap(etaI, etaT);
			H = -H;
		}
		float eta = etaI / etaT;
		// wo
		wo = glm::refract(wi, H, eta);
		if (glm::length(wo) < Consts::EPS) {
			return false;
		}

		float dotWoToNormal = dot(wo, normal);
		if (dotWiToNormal * dotWoToNormal < 0.0f) {
			return false;
		}

		// fr
		float dotWiToH = dot(wi, H);
		float dotWoToH = dot(wo, H);
		
		// factor = |wi.H|*|wo.H| / (|wi.Normal|*|wo.normal|)
		// denom = (-etaI*(wi.H)+etaT*(wo.H))^2
		// fr = factor * etaI^2*(1-F)*D*G / denom
		// pdf = (1-F)*D*(wh.normal) * (etaI^2*|wi.H|/denom)
		// cosine = wo.normal
		float geometry = GeometrySmith(abs(dotWiToH), abs(dotWoToH));
		fr = glm::vec3(geometry * abs(dotWiToH) / (abs(dotWiToNormal) + Consts::EPS));
		pdf = dot(wh, normal);
	}

	return true;
}

bool MaterialFrostedGlass::SampleWithImportance(const glm::vec3& normal, const glm::vec3& wi, glm::vec3& wo, float& pdf) {
	return true;
}

glm::vec3 MaterialFrostedGlass::Eval(const glm::vec3& normal, const glm::vec3& wi, const glm::vec3& wo) {
	float dotWiToNormal = dot(wi, normal);
	float dotWoToNormal = dot(wo, normal);
	if (dotWiToNormal * dotWoToNormal > 0) {
		// refract
		float etaI = 1.0f, etaT = mIor;
		if (dotWiToNormal > 0) {
			std::swap(etaI, etaT);
		}

		// ht
		glm::vec3 ht_N = normalize(etaI * wi - etaT * wo);	// 指向折射率低侧

		// fr
		float dotWiToHt = dot(wi, ht_N);
		float dotWoToHt = dot(wo, ht_N);

		float distribution = DistributionGGX(normal, ht_N);
		//float fresnel = Material::Fresnel(wi, ht_N, mIor);
		float fresnel = FresnelSchlic(wi, ht_N);
		float geometry = GeometrySmith(abs(dotWiToHt), abs(dotWoToHt));
		float factor = abs(dotWiToHt) * abs(dotWoToHt) / (abs(dotWiToNormal) + Consts::EPS);	// factor * cosine
		float denom = -etaI * dotWiToHt + etaT * dotWoToHt;
		denom = denom * denom;
		float nom = etaI * etaI * (1.0f - fresnel) * distribution * geometry;

		return glm::vec3(factor * nom / (denom + Consts::EPS));
	}
	else {
		// reflect
		glm::vec3 H = normalize(-wi + wo);	// 指向入射一侧
		glm::vec3 halfVector = H;
		if (dotWiToNormal > 0) {
			halfVector = -halfVector;		// 指向外侧
		}
		float Distribution = DistributionGGX(normal, halfVector);
		//float fresnel = Material::Fresnel(wi, halfVector, mIor);
		float fresnel = FresnelSchlic(wi, halfVector);
		float dotWiToH = dot(wi, halfVector);
		float dotWoToH = dot(wo, halfVector);
		float Geometry = GeometrySmith(abs(dotWiToH), abs(dotWoToH));
		return Distribution * glm::vec3(fresnel) * Geometry / (4.0f * abs(dotWiToNormal) + Consts::EPS);	// BRDF * cosine
	}
}

float MaterialFrostedGlass::DistributionGGX(glm::vec3 normal, glm::vec3 wh) {
	float dotNormalToWh = dot(normal, wh);
	float denom = Consts::M_PI * powf(dotNormalToWh * dotNormalToWh * (mAlphaSquare - 1.0f) + 1.0f, 2.0f);
	return mAlphaSquare / (denom);
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

float MaterialFrostedGlass::FresnelSchlic(glm::vec3 wi, glm::vec3 wh) {
	float cosTheta = abs(dot(wh, wi));
	if (dot(wi, wh) > 0.0f) {	// 从里往外射
		glm::vec3 rayInLowIor = glm::refract(wi, -wh, mIor);
		if (glm::length(rayInLowIor) < Consts::EPS) {
			return 1.0f;
		}
		cosTheta = abs(dot(rayInLowIor, wh));
	}

	
	//cosTheta = std::min(cosTheta, 1.0f);
	return mF0 + (1.0f - mF0) * powf(1.0f - cosTheta, 5.0f);
}