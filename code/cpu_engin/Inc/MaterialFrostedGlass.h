#ifndef MATERIAL_FROSTED_GLASS
#define MATERIAL_FROSTED_GLASS

#include "Material.h"
#include "TexureSampler.h"

class MaterialFrostedGlass : public Material
{
public:
	float mRoughness;
	float mAlpha;
	float mAlphaSquare;
	float mF0;
	float mIor;

	TexureSampler1F* mRoughnessTexure = nullptr;

public:
	MaterialFrostedGlass();
	MaterialFrostedGlass(float roughness, float ior);
	MaterialFrostedGlass(TexureSampler1F* roughnessTexure, float ior);
	virtual bool SampleAndEval(SampleData& data, TraceInfo info) override;
	virtual bool SampleWithImportance(SampleData& data) override;
	virtual void Eval(SampleData& data) override;
	bool IsExtremelySpecular(glm::vec2 texCoord) override;

	float DistributionGGX(glm::vec3 normal, glm::vec3 wh, float alphaSquare);
	float GeometrySchlickGGX(float dotNormalToW, float alphaSquare);
	float GeometrySmith(float absDotWiToNormal, float absDotWoToNormal, float alphaSquare);
	float FresnelSchlic(glm::vec3 wi, glm::vec3 wh);

public:
    void SetRoughness(float roughness);
    void SetRoughness(TexureSampler1F* roughnessTexure);
	void SetIor(float ior);
};

#endif // !MATERIAL_FROSTED_GLASS

