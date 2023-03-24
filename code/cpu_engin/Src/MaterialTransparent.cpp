#include "MaterialTransparent.h"

MaterialTransparent::MaterialTransparent() : Material(TRANSPARENT) {
	mIor = 1.0f;
	return;
}

MaterialTransparent::MaterialTransparent(float ior, glm::vec3 color) : Material(TRANSPARENT) {
	mIor = ior;
	mColor = color;
	return;
}

bool MaterialTransparent::SampleAndEval(SampleData& data, TraceInfo info) {
	float kr = Fresnel(data.wi, data.normal, mIor);
	float rand = Utils::GetUniformRandom();
	if (rand < kr) {
		// reflect
		if (dot(data.wi, data.normal) < 0.0f) {
			data.wo = normalize(glm::reflect(data.wi, data.normal));
		}
		else {
			data.wo = normalize(glm::reflect(data.wi, -data.normal));
		}
		
		data.pdf = 1.0f;
		data.frCosine = 1.0f * mColor;
	}
	else {
		// refract
		if (dot(data.wi, data.normal) < 0.0f) {
			float eta = 1.0f / mIor;
			data.wo = glm::refract(data.wi, data.normal, eta);
		}
		else {
			float eta = mIor;
			data.wo = glm::refract(data.wi, -data.normal, eta);
		}
		data.wo = glm::normalize(data.wo);
		data.pdf = 1.0f;
		data.frCosine = 1.0f * mColor;
	}
	return true;
}

bool MaterialTransparent::SampleWithImportance(SampleData& data) {
	return true;
}

void MaterialTransparent::Eval(SampleData& data) {
	float dotNtoWi = dot(data.normal, data.wi);
	float dotNtoWo = dot(data.normal, data.wo);
	if (dotNtoWi * dotNtoWo > 0) {
		// refract
		glm::vec3 ht_N;
		if (dotNtoWi > 0) {
			ht_N = normalize(mIor * data.wi - data.wo);
		}
		else {
			ht_N = normalize(data.wi - mIor * data.wo);
		}

		if (glm::length(ht_N - data.normal) < 0.5f) {
			data.frCosine = 1.0f * mColor;
			return;
		}
		
	}
	else {
		// reflect
		glm::vec3 halfVector = glm::normalize(-data.wi + data.wo);
		if (glm::length(halfVector - data.normal) < 0.1f) {
			data.frCosine = 1.0f * mColor;
			return;
		}
	}

	data.frCosine = glm::vec3(0.0f);
	return;
}

bool MaterialTransparent::IsExtremelySpecular(glm::vec2 texCoord) {
	return true;
}
