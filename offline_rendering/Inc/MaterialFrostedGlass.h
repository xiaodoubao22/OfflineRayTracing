#ifndef MATERIAL_FROSTED_GLASS
#define MATERIAL_FROSTED_GLASS

#include "Material.h"

class MaterialFrostedGlass : public Material
{
public:
	float mRoughness;
	float mAlpha;
	float mAlphaSquare;
	float mK;
	glm::vec3 mF0;
	float mIor;

public:
	MaterialFrostedGlass();
	explicit MaterialFrostedGlass(float roughness, float ior, glm::vec3 f0);
	virtual bool SampleAndEval(const glm::vec3& normal, const glm::vec3& wi, glm::vec3& wo, float& pdf, glm::vec3& fr) override;
	virtual bool SampleWithImportance(const glm::vec3& normal, const glm::vec3& wi, glm::vec3& wo, float& pdf) override;
	virtual glm::vec3 Eval(const glm::vec3& normal, const glm::vec3& wi, const glm::vec3& wo) override;

	float DistributionGGX(glm::vec3 normal, glm::vec3 wh);
	float GeometrySchlickGGX(float dotNormalToW);
	float GeometrySmith(float absDotWiToNormal, float absDotWoToNormal);
	glm::vec3 FresnelSchlic(float absDotWiToNormal);
};

#endif // !MATERIAL_FROSTED_GLASS

