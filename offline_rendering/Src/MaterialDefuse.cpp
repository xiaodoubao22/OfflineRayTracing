#include "MaterialDefuse.h"
#include "LdsGenerator.h"

MaterialDefuse::MaterialDefuse() : Material(DEFUSE) {
	return;
}

MaterialDefuse::MaterialDefuse(glm::vec3 albedo) : Material(DEFUSE) {
	mAlbedo = albedo;
	return;
}

MaterialDefuse::MaterialDefuse(TexureSampler3F* albedoTexure) : Material(DEFUSE) {
	mUseTexure = true;
	mAlbedoTexure = albedoTexure;
}

bool MaterialDefuse::SampleAndEval(SampleData& data, TraceInfo info) {
	if (dot(data.wi, data.normal) < 0.0f) {
		float x = LdsGenerator::GetInstance()->Get(info.depth * 2, info.threadNum);
		float y = LdsGenerator::GetInstance()->Get(info.depth * 2 + 1, info.threadNum);
		data.wo = Utils::RandomDirectionFromLDS(data.normal, x, y);
		//wo = Utils::randomDirection(normal);
		data.pdf = 1.0f / (2.0f * Consts::M_PI);
		float cosine = std::max(dot(data.wo, data.normal), 0.0f);
		glm::vec3 albedo = mUseTexure ? mAlbedoTexure->Sample(data.texCoord) : mAlbedo;
		data.frCosine = albedo / Consts::M_PI * cosine;
		return true;
	}

	return false;

}

bool MaterialDefuse::SampleWithImportance(SampleData& data) {
	return false;
}

void MaterialDefuse::Eval(SampleData& data) {
	float dotWiToNormal = dot(data.wi, data.normal);
	float dotWoToNormal = dot(data.wo, data.normal);
	if (dotWiToNormal < 0.0f && dotWoToNormal > 0.0f) {
		glm::vec3 albedo = mUseTexure ? mAlbedoTexure->Sample(data.texCoord) : mAlbedo;
		data.frCosine = albedo / Consts::M_PI * dotWoToNormal;
		return;
	}
	data.frCosine = glm::vec3(0.0f);
	return;
}


bool MaterialDefuse::IsExtremelySpecular(glm::vec2 texCoord) {
	return false;
}