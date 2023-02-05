#ifndef MATERIAL_COOK_TORRANCE
#define MATERIAL_COOK_TORRANCE

#include "Material.h"

class MaterialCookTorrance : public Material
{
public:
	float mRoughness;
	float mAlpha;
	float mAlphaSquare;
	float mK;
	glm::vec3 mF0;

public:
	MaterialCookTorrance();
	explicit MaterialCookTorrance(float roughness, glm::vec3 f0);
	virtual bool SampleAndEval(const glm::vec3& normal, const glm::vec3& wi, glm::vec3& wo, float& pdf, glm::vec3& fr) override;
	virtual bool SampleWithImportance(const glm::vec3& normal, const glm::vec3& wi, glm::vec3& wo, float& pdf) override;
	virtual glm::vec3 Eval(const glm::vec3& normal, const glm::vec3& wi, const glm::vec3& wo) override;
	
	float DistributionGGX(glm::vec3 normal, glm::vec3 wh);
	float GeometrySchlickGGX(float dotNormalToW);
	float GeometrySmith(float absDotWiToNormal, float absDotWoToNormal);
	glm::vec3 FresnelSchlic(float absDotWiToNormal);
};


#endif // !MATERIAL_COOK_TORRANCE

