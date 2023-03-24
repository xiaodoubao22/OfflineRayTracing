#ifndef MATERIAL_H
#define MATERIAL_H

#include "Utils.h"

struct SampleData {
	glm::vec3 wi = glm::vec3(0.0f);
	glm::vec3 wo = glm::vec3(0.0f);
	glm::vec3 normal = glm::vec3(0.0f);
	glm::vec2 texCoord = glm::vec2(0.0f);
	glm::vec3 frCosine = glm::vec3(0.0f);;	// frCosine = BRDF * cosine
	float pdf = 0.0f;
};

enum MaterialType {
	DEFUSE = 0,
	SPECULAR,
	TRANSPARENT,
	COOK_TORRANCE,
	FROSTED_GLASS
};

class Material {
public:
	MaterialType mType;
	bool mUseTexure = false;
	bool mHasEmission = false;
	glm::vec3 mEmissionRadiance = glm::vec3(0.0f, 0.0f, 0.0f);

public:
	Material() = delete;
	explicit Material(MaterialType type);
	MaterialType GetType();
	bool HasEmission();
	glm::vec3 GetEmission();
	static float Fresnel(glm::vec3 I, glm::vec3 N, float ior);
	static glm::vec3 LocalToWorld(glm::vec3 vec, glm::vec3 normal);

	// wo, wi from eye to scene
	// fr = BRDF * cosine
	virtual bool SampleAndEval(SampleData& data, TraceInfo info) = 0;
	virtual bool SampleWithImportance(SampleData& data) = 0;
	virtual void Eval(SampleData& data) = 0;
	virtual bool IsExtremelySpecular(glm::vec2 texCoord) = 0;
};

#endif // !MATERIAL_H
