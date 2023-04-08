#ifndef MATERIAL_COOK_TORRANCE
#define MATERIAL_COOK_TORRANCE

#include "Material.h"
#include "TexureSampler.h"

class MaterialCookTorrance : public Material
{
public:
    float mRoughness;
    float mAlpha;
    float mAlphaSquare;
    glm::vec3 mF0;

    TexureSampler1F* mRoughnessTexure = nullptr;

public:
    MaterialCookTorrance();
    MaterialCookTorrance(float roughness, glm::vec3 f0);
    MaterialCookTorrance(TexureSampler1F* roughnessTexure, glm::vec3 f0);
    virtual bool SampleAndEval(SampleData& data, TraceInfo info) override;
    virtual bool SampleWithImportance(SampleData& data) override;
    virtual void Eval(SampleData& data) override;
    bool IsExtremelySpecular(glm::vec2 texCoord) override;
    
    float DistributionGGX(glm::vec3 normal, glm::vec3 wh, float alphaSquare);
    float GeometrySchlickGGX(float dotNormalToW, float alphaSquare);
    float GeometrySmith(float absDotWiToNormal, float absDotWoToNormal, float alphaSquare);
    glm::vec3 FresnelSchlic(float absDotWiToNormal);

public:
    void SetRoughness(float roughness);
    void SetRoughness(TexureSampler1F* roughnessTexure);
	void SetF0(const glm::vec3& f0);
};


#endif // !MATERIAL_COOK_TORRANCE

