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
    float mFavg;
	float mIor;

	TexureSampler* mRoughnessTexure = nullptr;

    // kulla-county
    static const int mEmuListSize = 128;   // (h=thetaO, w=roughness)
    static const int mESample = 1024;        // 积分采样次数
    static TexureSampler2D<float> mEmuList;
    static TexureSampler2D<float> mEavgList;

public:
	MaterialFrostedGlass();
	MaterialFrostedGlass(float roughness, float ior);
	MaterialFrostedGlass(TexureSampler* roughnessTexure, float ior);
    virtual ~MaterialFrostedGlass() override;

	virtual bool SampleAndEval(SampleData& data, TraceInfo info) override;
	virtual bool SampleWithImportance(SampleData& data, const TraceInfo& info) override;
	virtual void Eval(SampleData& data) override;
	bool IsExtremelySpecular(glm::vec2 texCoord) override;

	float DistributionGGX(glm::vec3 normal, glm::vec3 wh, float alphaSquare);
	float GeometrySchlickGGX(float dotNormalToW, float alphaSquare);
	float GeometrySmith(float absDotWiToNormal, float absDotWoToNormal, float alphaSquare);
	float FresnelSchlic(glm::vec3 wi, glm::vec3 wh);

    glm::vec3 ImportanceSampleGGX(glm::vec2 Xi, glm::vec3 wi, float alphaSquare);
    void GenerateKullaCountyMap();

public:
    void SetRoughness(float roughness);
    void SetRoughness(TexureSampler* roughnessTexure);
	void SetIor(float ior);
};

#endif // !MATERIAL_FROSTED_GLASS

