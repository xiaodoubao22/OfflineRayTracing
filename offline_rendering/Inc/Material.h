#ifndef MATERIAL_H
#define MATERIAL_H

#include "Utils.h"

//class Material {
//public:
//	bool isEmission = false;
//	glm::vec3 emission = glm::vec3(0.0f, 0.0f, 0.0f);
//	glm::vec3 albedo = glm::vec3(0.0f, 0.0f, 0.0f);
//	float specularRate = 0.0f;
//	float roughness = 0.0f;
//	float refractRate = 0.0f;
//	float ior = 1.0f;
//
//};

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
	virtual bool SampleAndEval(const glm::vec3& normal, const glm::vec3& wi, glm::vec3& wo, float& pdf, glm::vec3& fr) = 0;
	virtual bool SampleWithImportance(const glm::vec3& normal, const glm::vec3& wi, glm::vec3& wo, float& pdf) = 0;
	virtual glm::vec3 Eval(const glm::vec3& normal, const glm::vec3& wi, const glm::vec3& wo) = 0;
};

#endif // !MATERIAL_H
