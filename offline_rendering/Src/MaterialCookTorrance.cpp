#include "MaterialCookTorrance.h"
#include "LdsGenerator.h"

MaterialCookTorrance::MaterialCookTorrance() : Material(COOK_TORRANCE) {
	mF0 = glm::vec3(1.0f);
	mRoughness = 0.5f;
	mAlpha = mRoughness * mRoughness;
	mAlphaSquare = mAlpha * mAlpha;
}

MaterialCookTorrance::MaterialCookTorrance(float roughness, glm::vec3 f0) : Material(COOK_TORRANCE) {
	mF0 = f0;
	mRoughness = roughness;
	mAlpha = mRoughness * mRoughness;
	mAlphaSquare = mAlpha * mAlpha;
}

MaterialCookTorrance::MaterialCookTorrance(TexureSampler1F* roughnessTexure, glm::vec3 f0) : Material(COOK_TORRANCE) {
	mUseTexure = true;
	mRoughnessTexure = roughnessTexure;
	mF0 = f0;
}

bool MaterialCookTorrance::SampleAndEval(SampleData& data, TraceInfo info) {
	float dotWiToNormal = dot(data.wi, data.normal);
	if (dotWiToNormal < 0.0f) {
		// sample wo
		//float phi = Utils::GetUniformRandom(0, 2 * Consts::M_PI);
		//float ksi = Utils::GetUniformRandom();
		float phi = LdsGenerator::GetInstance()->Get(info.depth * 2, info.threadNum) * 2.0f * Consts::M_PI;
		float ksi = LdsGenerator::GetInstance()->Get(info.depth * 2 + 1, info.threadNum);
		float alphaSquare = mUseTexure ? pow(mRoughnessTexure->Sample(data.texCoord), 4) : mAlphaSquare;
		float cosThetaSquare = (1.0f - ksi) / (ksi * (alphaSquare - 1.0f) + 1.0f);
		float cosTheta = std::sqrt(cosThetaSquare);
		float sinTheta = std::sqrt(1.0f - cosThetaSquare);
		glm::vec3 localWh(sinTheta * std::cos(phi), sinTheta * std::sin(phi), cosTheta);
		glm::vec3 wh = LocalToWorld(localWh, data.normal);
		data.wo = glm::reflect(data.wi, wh);

		// fr
		float dotWoToNormal = dot(data.wo, data.normal);
		float Distribution = DistributionGGX(data.normal, wh, alphaSquare);
		float dotWiToH = dot(data.wi, wh);
		glm::vec3 Fresnel = FresnelSchlic(abs(dotWiToH));
		//float fresnel = Material::Fresnel(wi, wh, 1.5f);
		//glm::vec3 Fresnel(fresnel);
		float Geometry = GeometrySmith(abs(dotWiToNormal), abs(dotWoToNormal), alphaSquare);
		data.frCosine = Distribution * Fresnel * Geometry / (4.0f * abs(dotWiToNormal) + Consts::EPS);	// BRDF * cosine

		// pdf
		data.pdf = Distribution * dot(wh, data.normal) / (4.0f * abs(dot(data.wi, wh)) + Consts::EPS);

		return true;
	}
	return false;
}

bool MaterialCookTorrance::SampleWithImportance(SampleData& data) {
	return false;
}

void MaterialCookTorrance::Eval(SampleData& data) {
	float dotWiToNormal = dot(data.wi, data.normal);
	float dotWoToNormal = dot(data.wo, data.normal);
	if (dotWiToNormal < 0.0f && dotWoToNormal > 0.0f) {
		glm::vec3 halfVector = normalize(-data.wi + data.wo);

		float alphaSquare = mUseTexure ? pow(mRoughnessTexure->Sample(data.texCoord), 4.0f) : mAlphaSquare;
		//std::cout << mRoughnessTexure->Sample(data.texCoord) << std::endl;
		float Distribution = DistributionGGX(data.normal, halfVector, alphaSquare);
		float dotWiToH = dot(data.wi, halfVector);
		glm::vec3 Fresnel = FresnelSchlic(abs(dotWiToH));
		//float fresnel = Material::Fresnel(wi, halfVector, 1.5f);
		//glm::vec3 Fresnel(fresnel);
		float Geometry = GeometrySmith(abs(dotWiToNormal), abs(dotWoToNormal), alphaSquare);

		data.frCosine = Distribution * Fresnel * Geometry / (4.0f * abs(dotWiToNormal));	// BRDF * cosine
	}
	else {
		data.frCosine = glm::vec3(0.0f);
	}
	return;
}

bool MaterialCookTorrance::IsExtremelySpecular(glm::vec2 texCoord) {
	float roughness = mUseTexure ? mRoughnessTexure->Sample(texCoord) : mRoughness;
	return roughness < 0.12;
}

float MaterialCookTorrance::DistributionGGX(glm::vec3 normal, glm::vec3 wh, float alphaSquare) {
	float dotNormalToWh = dot(normal, wh);
	float denom = Consts::M_PI * powf(dotNormalToWh * dotNormalToWh * (alphaSquare - 1.0f) + 1.0f, 2.0f);
	return alphaSquare / denom;
}

float MaterialCookTorrance::GeometrySchlickGGX(float dotNormalToW, float alphaSquare) {
	float cosThetaSquare = dotNormalToW * dotNormalToW;
	float sinThetaSquare = 1.0f - cosThetaSquare;
	float tanThetaSquare = sinThetaSquare / cosThetaSquare;
	float lamda = (sqrt(1.0f + alphaSquare * tanThetaSquare) - 1.0f) / 2.0f;
	return 1.0f / (1.0f + lamda);
}

float MaterialCookTorrance::GeometrySmith(float absDotWiToNormal, float absDotWoToNormal, float alphaSquare) {
	float G1 = GeometrySchlickGGX(absDotWiToNormal, alphaSquare);
	float G2 = GeometrySchlickGGX(absDotWoToNormal, alphaSquare);
	return G1 * G2;
}

glm::vec3 MaterialCookTorrance::FresnelSchlic(float absDotWiToNormal) {
	float cosTheta = std::min(absDotWiToNormal, 1.0f);
	return mF0 + (glm::vec3(1.0f) - mF0) * powf(1.0f - cosTheta, 5.0f);
}