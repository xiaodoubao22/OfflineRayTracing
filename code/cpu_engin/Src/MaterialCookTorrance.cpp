#include "MaterialCookTorrance.h"
#include "LdsGenerator.h"

TexureSampler2D<float> MaterialCookTorrance::mEmuList;
TexureSampler2D<float> MaterialCookTorrance::mEavgList;

glm::vec3 ImportanceSampleGGX(glm::vec2 Xi, glm::vec3 N, float alphaSquare) {
    float cosThetaSquare = (1.0f - Xi.x) / (Xi.x * (alphaSquare - 1.0f) + 1.0f);
    float cosTheta = std::sqrt(cosThetaSquare);
    float sinTheta = std::sqrt(1.0f - cosThetaSquare);
    float phi = 2.0f * Consts::PI * Xi.y;
    return glm::vec3(sinTheta * std::cos(phi), sinTheta * std::sin(phi), cosTheta);
}

MaterialCookTorrance::MaterialCookTorrance() : Material(COOK_TORRANCE) {
    mF0 = glm::vec3(1.0f);
    mRoughness = 0.5f;
    mAlpha = mRoughness * mRoughness;
    mAlphaSquare = mAlpha * mAlpha;
}

MaterialCookTorrance::MaterialCookTorrance(float roughness, glm::vec3 f0) : Material(COOK_TORRANCE) {
    mF0 = f0;
    mRoughness = roughness;
    mAlpha = mRoughness * mRoughness;
    mAlphaSquare = mAlpha * mAlpha;
}

MaterialCookTorrance::MaterialCookTorrance(TexureSampler2D<float> roughnessTexure, glm::vec3 f0) : Material(COOK_TORRANCE) {
    mF0 = f0;
    mUseTexure = true;
    mRoughnessTexure = roughnessTexure;
}

MaterialCookTorrance::~MaterialCookTorrance() {

}

bool MaterialCookTorrance::SampleAndEval(SampleData& data, TraceInfo info) {
    float dotWiToNormal = dot(data.wi, data.normal);
    if (dotWiToNormal < 0.0f) {
        // sample wo
        float ksi1, ksi2;
        if (data.aRandomGenerator != nullptr) {
            ksi1 = data.aRandomGenerator->Get(info.depth * 2, info.pixelNum);
            ksi2 = data.aRandomGenerator->Get(info.depth * 2 + 1, info.pixelNum);
        }
        else {
            ksi1 = Utils::GetUniformRandom();
            ksi2 = Utils::GetUniformRandom();
        }
        float alphaSquare = mUseTexure ? pow(mRoughnessTexure.Sample(data.texCoord.x, data.texCoord.y), 4) : mAlphaSquare;
        glm::vec3 localWh = ImportanceSampleGGX(glm::vec2(ksi1, ksi2), data.normal, alphaSquare);
        data.wh = LocalToWorld(localWh, data.normal);
        data.wo = glm::reflect(data.wi, data.wh);

        // fr
        float dotWoToNormal = dot(data.wo, data.normal);
        float Distribution = DistributionGGX(data.normal, data.wh, alphaSquare);
        float dotWiToH = dot(data.wi, data.wh);
        glm::vec3 Fresnel = FresnelSchlic(abs(dotWiToH));
        float Geometry = GeometrySmith(abs(dotWiToNormal), abs(dotWoToNormal), alphaSquare);
        glm::vec3 fr = Distribution * Fresnel * Geometry / std::max(4.0f * abs(dotWiToNormal) * abs(dotWoToNormal), Consts::EPS);
        
        // kulla-county
        float emuI = mEmuList.Sample(abs(dotWiToNormal), mRoughness);
        float emuO = mEmuList.Sample(abs(dotWoToNormal), mRoughness);
        float eAvg = mEavgList.Sample(0.0f, mRoughness);
        float fms = (1.0f - emuI) * (1.0f - emuO) / std::max(Consts::PI * (1.0f - eAvg), Consts::EPS);
        data.frCosine = (fr + glm::vec3(fms)) * abs(dotWoToNormal);
        // std::cout << mEList[tiIndex * mEmuListSize.y + rouhnessIndex].x << " " << mEList[tiIndex * mEmuListSize.y + rouhnessIndex].y << " " << mEList[tiIndex * mEmuListSize.y + rouhnessIndex].z << " " << toIndex << " " << tiIndex << " " << rouhnessIndex << std::endl;
        // data.frCosine = Distribution * Fresnel * Geometry / std::max(4.0f * abs(dotWiToNormal), Consts::EPS);    // BRDF * cosine

        // pdf
        data.pdf = Distribution/* * glm::dot(data.wh, data.normal)*/ / std::max(4.0f * abs(dot(data.wo, data.wh)), Consts::EPS);

        return true;
    }
    return false;
}

bool MaterialCookTorrance::SampleWithImportance(SampleData& data, const TraceInfo& info) {
    float dotWiToNormal = dot(data.wi, data.normal);
    if (dotWiToNormal > 0.0f) {
        return false;
    }

    // sample wo wh
    float ksi1, ksi2;
    if (data.aRandomGenerator != nullptr) {
        ksi1 = data.aRandomGenerator->Get(info.depth * 2, info.pixelNum);
        ksi2 = data.aRandomGenerator->Get(info.depth * 2 + 1, info.pixelNum);
    }
    else {
        ksi1 = Utils::GetUniformRandom();
        ksi2 = Utils::GetUniformRandom();
    }
    float alphaSquare = mUseTexure ? pow(mRoughnessTexure.Sample(data.texCoord.x, data.texCoord.y), 4) : mAlphaSquare;
    glm::vec3 localWh = ImportanceSampleGGX(glm::vec2(ksi1, ksi2), data.normal, alphaSquare);
    data.wh = LocalToWorld(localWh, data.normal);
    data.wo = glm::reflect(data.wi, data.wh);

    // pdf
    float Distribution = DistributionGGX(data.normal, data.wh, alphaSquare);
    data.pdf = Distribution * glm::dot(data.wh, data.normal) / std::max(4.0f * abs(glm::dot(data.wi, data.wh)), Consts::EPS);
    return true;
}

void MaterialCookTorrance::Eval(SampleData& data) {
    float dotWiToNormal = dot(data.wi, data.normal);
    float dotWoToNormal = dot(data.wo, data.normal);
    if (dotWiToNormal < 0.0f && dotWoToNormal > 0.0f) {
        glm::vec3 halfVector = normalize(-data.wi + data.wo);

        float alphaSquare = mUseTexure ? pow(mRoughnessTexure.Sample(data.texCoord.x, data.texCoord.y), 4.0f) : mAlphaSquare;
        //std::cout << mRoughnessTexure->Sample(data.texCoord) << std::endl;
        float Distribution = DistributionGGX(data.normal, halfVector, alphaSquare);
        float dotWiToH = dot(data.wi, halfVector);
        glm::vec3 Fresnel = FresnelSchlic(abs(dotWiToH));
        //float fresnel = Material::Fresnel(wi, halfVector, 1.5f);
        //glm::vec3 Fresnel(fresnel);
        float Geometry = GeometrySmith(abs(dotWiToNormal), abs(dotWoToNormal), alphaSquare);

        data.frCosine = Distribution * Fresnel * Geometry / (4.0f * abs(dotWiToNormal));    // BRDF * cosine
    }
    else {
        data.frCosine = glm::vec3(0.0f);
    }
    return;
}

bool MaterialCookTorrance::IsExtremelySpecular(glm::vec2 texCoord) {
    float roughness = mUseTexure ? mRoughnessTexure.Sample(texCoord.x, texCoord.y) : mRoughness;
    return roughness < 0.05;
}

float MaterialCookTorrance::DistributionGGX(glm::vec3 normal, glm::vec3 wh, float alphaSquare) {
    float dotNormalToWh = dot(normal, wh);
    float denom = Consts::PI * powf(dotNormalToWh * dotNormalToWh * (alphaSquare - 1.0f) + 1.0f, 2.0f);
    return alphaSquare / (denom + Consts::EPS);
}

float MaterialCookTorrance::DistributionGGX(float dotNormalToWh, float alphaSquare) {
    float denom = Consts::PI * powf(dotNormalToWh * dotNormalToWh * (alphaSquare - 1.0f) + 1.0f, 2.0f);
    return alphaSquare / denom;
}

float MaterialCookTorrance::GeometrySchlickGGX(float dotNormalToW, float alphaSquare) {
    float cosThetaSquare = dotNormalToW * dotNormalToW;
    float sinThetaSquare = 1.0f - cosThetaSquare;
    float tanThetaSquare = sinThetaSquare / cosThetaSquare;
    float lamda = (std::sqrt(1.0f + alphaSquare * tanThetaSquare) - 1.0f) / 2.0f;
    return 1.0f / (1.0f + lamda);

    // float k = mRoughness * mRoughness / 2.0f;
    // return dotNormalToW / (dotNormalToW * (1.0f - k) + k);
}

float MaterialCookTorrance::GeometrySmith(float absDotWiToNormal, float absDotWoToNormal, float alphaSquare) {
    float G1 = GeometrySchlickGGX(absDotWiToNormal, alphaSquare);
    float G2 = GeometrySchlickGGX(absDotWoToNormal, alphaSquare);
    return G1 * G2;
}

glm::vec3 MaterialCookTorrance::FresnelSchlic(float absDotWiToNormal) {
    float cosTheta = std::min(absDotWiToNormal, 1.0f);
    return mF0 + (glm::vec3(1.0f) - mF0) * powf(1.0f - cosTheta, 5.0f);
}

void MaterialCookTorrance::GenerateKullaCountyMap() {
    if (mEmuList.Width() * mEavgList.Height() != 0) {
        return;
    }

    LdsGenerator aGen(1, 2);
    mEmuList = TexureSampler2D<float>(mEmuListSize, mEmuListSize);
    mEavgList = TexureSampler2D<float>(1, mEmuListSize);

    for (int i = 0; i < mEmuListSize; i++) {            // roughness
        for (int j = 0; j < mEmuListSize; j++) {        // theta
            // roughness
            float roughness = ((float)i + 0.5f) / mEmuListSize;
            float alpha = roughness * roughness;
            float alphaSquare = alpha * alpha;
        
            // cosTheta
            float dotWiToN = ((float)j + 0.5f) / mEmuListSize;       // [0, 1)
            glm::vec3 wi = glm::vec3(std::sqrt(1.0f - dotWiToN * dotWiToN), 0.0f, dotWiToN);

            // integrate
            float e(0.0f);
            aGen.Reset(0, 0);
            aGen.Reset(1, 0);
            for(int sample = 0; sample < mESample; sample++) {
                float ksi1 = aGen.Get(0, 0);
                float ksi2 = aGen.Get(1, 0);
                glm::vec3 wh = ImportanceSampleGGX(glm::vec2(ksi1, ksi2), Consts::Z_AXIS, alphaSquare);
                glm::vec3 wo = glm::reflect(-wi, wh);

                float dotWiToN = std::max(wi.z, 0.0f);
                float dotWoToN = std::max(wo.z, 0.0f);
                float dotWhToN = std::max(wh.z, 0.0f);
                float dotWiToWh = std::max(dot(wi, wh), 0.0f);

                float Geometry = GeometrySmith(abs(dotWoToN), abs(dotWiToN), alphaSquare);
                e += (Geometry * dotWiToWh) / (dotWhToN * dotWiToN);
            }
            e /= mESample;
            mEmuList.SetPixel(j, i, e);
        }
    }
    Utils::SaveImage(mEmuList.GetData(), "../../result/emu.png", mEmuListSize, mEmuListSize, 1);

    for (int i = 0; i < mEmuListSize; i++) {     // roughness
        float eAvg= 0.0f;
        for(int j = 0; j < mEmuListSize; j++) {  // 均匀采样 sinT
            float dotWiToN = ((float)j + 0.5f) / mEmuListSize; 
            eAvg += mEmuList.Sample(j, i) * dotWiToN * 2.0f / mEmuListSize;
        }
        mEavgList.SetPixel(0, i, eAvg);
    }

    Utils::SaveImage(mEavgList.GetData(), "../../result/eavg.png", 1, mEmuListSize, 1);
}

void MaterialCookTorrance::SetRoughness(float roughness) {
    mRoughness = roughness;
    mAlpha = mRoughness * mRoughness;
    mAlphaSquare = mAlpha * mAlpha;
}

void MaterialCookTorrance::SetRoughness(TexureSampler2D<float> roughnessTexure) {
    mRoughnessTexure = roughnessTexure;
}

void MaterialCookTorrance::SetF0(const glm::vec3& f0) {
    mF0 = f0;
}
