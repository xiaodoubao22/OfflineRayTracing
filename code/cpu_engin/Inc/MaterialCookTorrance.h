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
    glm::vec3 mFavg;

    TexureSampler2D<float> mRoughnessTexure;

    // kulla-county
    static const int mEmuListSize = 128;   // (h=thetaO, w=roughness)
    static const int mESample = 1024;        // 积分采样次数
    static TexureSampler2D<float> mEmuList;
    static TexureSampler2D<float> mEavgList;

public:
    MaterialCookTorrance();
    MaterialCookTorrance(float roughness, glm::vec3 f0);
    MaterialCookTorrance(TexureSampler2D<float> roughnessTexure, glm::vec3 f0);
    virtual ~MaterialCookTorrance() override;
    virtual bool SampleAndEval(SampleData& data, TraceInfo info) override;
    virtual bool SampleWithImportance(SampleData& data, const TraceInfo& info) override;
    virtual void Eval(SampleData& data) override;
    bool IsExtremelySpecular(glm::vec2 texCoord) override;
    
    float DistributionGGX(glm::vec3 normal, glm::vec3 wh, float alphaSquare);
    float DistributionGGX(float dotNormalToWh, float alphaSquare);
    float GeometrySchlickGGX(float dotNormalToW, float alphaSquare);
    float GeometrySmith(float absDotWiToNormal, float absDotWoToNormal, float alphaSquare);
    glm::vec3 FresnelSchlic(float absDotWiToNormal);

    void GenerateKullaCountyMap();

public:
    void SetRoughness(float roughness);
    void SetRoughness(TexureSampler2D<float> roughnessTexure);
	void SetF0(const glm::vec3& f0);

};


#endif // !MATERIAL_COOK_TORRANCE

