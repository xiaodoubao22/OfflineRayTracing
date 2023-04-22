#include "MaterialSpecular.h"

MaterialSpecular::MaterialSpecular() : Material(SPECULAR) {
	mSpecularRate = glm::vec3(1.0f);
	return;
}

MaterialSpecular::MaterialSpecular(glm::vec3 specularRate) : Material(SPECULAR) {
	mSpecularRate = specularRate;
	return;
}

MaterialSpecular::~MaterialSpecular() {

}

bool MaterialSpecular::SampleAndEval(SampleData& data, TraceInfo info) {
	float dotWiToNormal = dot(data.wi, data.normal);
	if (dotWiToNormal < 0.0f) {
		data.wo = normalize(glm::reflect(data.wi, data.normal));
		data.pdf = 1.0f;
		data.frCosine = mSpecularRate;
		return true;
	}
	return false;
}

bool MaterialSpecular::SampleWithImportance(SampleData& data, const TraceInfo& info) {
	if (dot(data.wi, data.normal) < 0.0f) {
		data.wo = normalize(glm::reflect(data.wi, data.normal));
		data.pdf = 1.0f;
		return true;
	}
	return false;
}

void MaterialSpecular::Eval(SampleData& data) {
	if (dot(data.wi, data.normal) <= 0.0f && dot(data.wo, data.normal) >= 0.0f) {
		glm::vec3 halfVector = glm::normalize(-data.wi + data.wo);
		if (glm::length(halfVector - data.normal) < 0.1f) {
			data.frCosine = mSpecularRate;
			return;
		}
	}
	data.frCosine = glm::vec3(0.0f);
	return;
}

bool MaterialSpecular::IsExtremelySpecular(glm::vec2 texCoord) {
	return true;
}