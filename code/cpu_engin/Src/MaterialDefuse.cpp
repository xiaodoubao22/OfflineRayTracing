#include "MaterialDefuse.h"
#include "LdsGenerator.h"

MaterialDefuse::MaterialDefuse() : Material(DEFUSE) {
	return;
}

MaterialDefuse::MaterialDefuse(glm::vec3 albedo) : Material(DEFUSE) {
	mAlbedo = albedo;
	return;
}

MaterialDefuse::MaterialDefuse(TexureSampler* albedoTexure) : Material(DEFUSE) {
	mUseTexure = true;
	mAlbedoTexure = albedoTexure;
}

MaterialDefuse::~MaterialDefuse() {
}

bool MaterialDefuse::SampleAndEval(SampleData& data, TraceInfo info) {
	if (dot(data.wi, data.normal) < 0.0f) {
        if (data.aRandomGenerator != nullptr) {
            float x = data.aRandomGenerator->Get(info.depth * 2, info.pixelNum);
            float y = data.aRandomGenerator->Get(info.depth * 2 + 1, info.pixelNum);
            data.wo = Utils::RandomDirectionFromLDS(data.normal, x, y);
        }
        else {
            data.wo = Utils::randomDirection(data.normal);
        }

		data.pdf = 1.0f / (2.0f * Consts::PI);
		float cosine = std::max(dot(data.wo, data.normal), 0.0f);
		glm::vec3 albedo = mUseTexure ? mAlbedoTexure->Sample(data.texCoord) : mAlbedo;
		data.frCosine = albedo / Consts::PI * cosine;
		return true;
	}

	return false;

}

bool MaterialDefuse::SampleWithImportance(SampleData& data, const TraceInfo& info) {
	return false;
}

void MaterialDefuse::Eval(SampleData& data) {
	float dotWiToNormal = dot(data.wi, data.normal);
	float dotWoToNormal = dot(data.wo, data.normal);
	if (dotWiToNormal < 0.0f && dotWoToNormal > 0.0f) {
		glm::vec3 albedo = mUseTexure ? mAlbedoTexure->Sample(data.texCoord) : mAlbedo;
		data.frCosine = albedo / Consts::PI * dotWoToNormal;
		return;
	}
	data.frCosine = glm::vec3(0.0f);
	return;
}


bool MaterialDefuse::IsExtremelySpecular(glm::vec2 texCoord) {
	return false;
}

void MaterialDefuse::SetAlbedo(const glm::vec3& albedo) {
	mAlbedo = albedo;
}

void MaterialDefuse::SetAlbedo(TexureSampler* albedoTexure) {
	mAlbedoTexure = albedoTexure;
	mUseTexure = true;
}
