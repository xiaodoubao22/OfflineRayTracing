#include "MaterialFrostedGlass.h"
#include "LdsGenerator.h"

TexureSampler2D<float> MaterialFrostedGlass::mEmuList;
TexureSampler2D<float> MaterialFrostedGlass::mEavgList;

glm::vec3 MaterialFrostedGlass::ImportanceSampleGGX(glm::vec2 Xi, glm::vec3 wi, float alphaSquare) {
    float cosThetaSquare = (1.0f - Xi.x) / (Xi.x * (alphaSquare - 1.0f) + 1.0f);
    float cosTheta = std::sqrt(cosThetaSquare);
    float sinTheta = std::sqrt(1.0f - cosThetaSquare);
    float phi = 2.0f * Consts::PI * Xi.y;
    return glm::vec3(sinTheta * std::cos(phi), sinTheta * std::sin(phi), cosTheta);
}

MaterialFrostedGlass::MaterialFrostedGlass() : Material(FROSTED_GLASS) {
    mF0 = 0.04f;
    mRoughness = 0.5f;
    mAlpha = mRoughness * mRoughness;
    mAlphaSquare = mAlpha * mAlpha;
}

MaterialFrostedGlass::MaterialFrostedGlass(float roughness, float ior) : Material(FROSTED_GLASS) {
    mIor = ior;
    mF0 = pow((1.0f - ior) / (1.0f + ior), 2);
    mRoughness = roughness;
    mAlpha = mRoughness * mRoughness;
    mAlphaSquare = mAlpha * mAlpha;
}

MaterialFrostedGlass::MaterialFrostedGlass(TexureSampler* roughnessTexure, float ior) : Material(FROSTED_GLASS) {
    mUseTexure = true;
    mIor = ior;
    mF0 = pow((1.0f - ior) / (1.0f + ior), 2);
    mRoughnessTexure = roughnessTexure;
}

MaterialFrostedGlass::~MaterialFrostedGlass() {
}

bool MaterialFrostedGlass::SampleAndEval(SampleData& data, TraceInfo info) {
    // sample wh

    float phi, ksi;
    if (data.aRandomGenerator != nullptr) {
        phi = data.aRandomGenerator->Get(info.depth * 2, info.pixelNum) * 2.0f * Consts::PI;
        ksi = data.aRandomGenerator->Get(info.depth * 2 + 1, info.pixelNum);
    }
    else {
        phi = Utils::GetUniformRandom(0, 2 * Consts::PI);
        ksi = Utils::GetUniformRandom();
    }

    float alphaSquare = mUseTexure ? pow(mRoughnessTexure->Sample(data.texCoord).x, 4) : mAlphaSquare;
    float cosThetaSquare = (1.0f - ksi) / (ksi * (alphaSquare - 1.0f) + 1.0f);
    float cosTheta = std::sqrt(cosThetaSquare);
    float sinTheta = std::sqrt(1.0f - cosThetaSquare);
    glm::vec3 localWh(sinTheta * std::cos(phi), sinTheta * std::sin(phi), cosTheta);
    glm::vec3 wh = LocalToWorld(localWh, data.normal);
    //float distribution = DistributionGGX(normal, wh);

    // calculate wo and pdf
    float dotWiToNormal = dot(data.wi, data.normal);
    // choose reflect or refract according to fresnel term
    //float fresnel = Material::Fresnel(wi, wh, mIor);
    float fresnel = FresnelSchlic(data.wi, wh);
    float rand = Utils::GetUniformRandom();
    if (rand < fresnel) {
        // reflect
        // wo
        glm::vec3 H = wh;    // 指向入射侧，专供refract使用
        if (dotWiToNormal > 0) {
            H = -H;
        }

        data.wo = glm::normalize(glm::reflect(data.wi, H));
        float dotWoToNormal = dot(data.wo, data.normal);
        if (dotWiToNormal * dotWoToNormal > 0.0f) {
            data.pdf = 0.0f;
            return false;
        }
        float dotWiToH = dot(data.wi, wh);
        float dotWoToH = dot(data.wo, wh);

        // fr
        float geometry = GeometrySmith(abs(dotWiToNormal), abs(dotWoToNormal), alphaSquare);

        float frFactor = /*fresnel * distribution * */geometry / (4.0f * abs(dotWiToNormal) + Consts::EPS);    // BRDF * cosine
        data.frCosine = glm::vec3(frFactor);

        // pdf
        data.pdf = /*fresnel * distribution * */dot(wh, data.normal) / (4.0f * abs(dotWiToH) + Consts::EPS);
    }
    else {
        // refract
        float etaI = 1.0f, etaT = mIor;
        glm::vec3 H = wh;    // 指向入射侧，专供refract使用
        if (dotWiToNormal > 0) {
            std::swap(etaI, etaT);
            H = -H;
        }
        float eta = etaI / etaT;
        // wo
        data.wo = glm::refract(data.wi, H, eta);
        if (glm::length(data.wo) < Consts::EPS) {
            return false;
        }

        float dotWoToNormal = dot(data.wo, data.normal);
        if (dotWiToNormal * dotWoToNormal < 0.0f) {
            return false;
        }

        // fr
        float dotWiToH = dot(data.wi, H);
        float dotWoToH = dot(data.wo, H);
        
        // factor = |wi.H|*|wo.H| / (|wi.Normal|*|wo.normal|)
        // denom = (-etaI*(wi.H)+etaT*(wo.H))^2
        // fr = factor * etaI^2*(1-F)*D*G / denom
        // pdf = (1-F)*D*(wh.normal) * (etaI^2*|wi.H|/denom)
        // cosine = wo.normal
        float geometry = GeometrySmith(abs(dotWiToNormal), abs(dotWoToNormal), alphaSquare);
        data.frCosine = glm::vec3(geometry * abs(dotWiToH) * (1.0f - fresnel) / (abs(dotWiToNormal) + Consts::EPS));
        data.pdf = dot(wh, data.normal);
    }

    return true;

}

bool MaterialFrostedGlass::SampleWithImportance(SampleData& data, const TraceInfo& info) {
    return true;
}

void MaterialFrostedGlass::Eval(SampleData& data) {
    float dotWiToNormal = dot(data.wi, data.normal);
    float dotWoToNormal = dot(data.wo, data.normal);
    if (dotWiToNormal * dotWoToNormal < 0) {
        //return glm::vec3(0.0f);
        // reflect
        glm::vec3 H = normalize(-data.wi + data.wo);    // 指向入射一侧
        glm::vec3 halfVector = H;
        if (dotWiToNormal > 0) {
            halfVector = -halfVector;        // 指向外侧
        }
        float alphaSquare = mUseTexure ? pow(mRoughnessTexure->Sample(data.texCoord).x, 4) : mAlphaSquare;
        float Distribution = DistributionGGX(data.normal, halfVector, alphaSquare);
        float fresnel = FresnelSchlic(data.wi, halfVector);
        float dotWiToH = dot(data.wi, halfVector);
        float dotWoToH = dot(data.wo, halfVector);
        float Geometry = GeometrySmith(abs(dotWiToNormal), abs(dotWoToNormal), alphaSquare);
        data.frCosine = Distribution * glm::vec3(fresnel) * Geometry / (4.0f * abs(dotWiToNormal) + Consts::EPS);    // BRDF * cosine
    }
    else {
        // refract
        float etaI = 1.0f, etaT = mIor;
        if (dotWiToNormal > 0) {
            std::swap(etaI, etaT);
        }

        // ht
        glm::vec3 ht_N = normalize(etaI * data.wi - etaT * data.wo);    // 指向折射率低侧

        // fr
        float dotWiToHt = dot(data.wi, ht_N);
        float dotWoToHt = dot(data.wo, ht_N);

        float alphaSquare = mUseTexure ? pow(mRoughnessTexure->Sample(data.texCoord).x, 4) : mAlphaSquare;
        float distribution = DistributionGGX(data.normal, ht_N, alphaSquare);
        float fresnel = FresnelSchlic(data.wi, ht_N);
        float geometry = GeometrySmith(abs(dotWiToNormal), abs(dotWoToNormal), alphaSquare);
        float factor = abs(dotWiToHt)* abs(dotWoToHt) / (abs(dotWiToNormal)+ Consts::EPS);    // factor * cosine
        float denom = -etaI * dotWiToHt + etaT * dotWoToHt;
        denom = denom * denom;
        float nom = etaI * etaI * (1.0f - fresnel) * distribution * geometry;

        data.frCosine = glm::vec3(factor * nom / (denom + Consts::EPS));
    }
    return;
}

bool MaterialFrostedGlass::IsExtremelySpecular(glm::vec2 texCoord) {
    float roughness = mUseTexure ? mRoughnessTexure->Sample(texCoord).x : mRoughness;
    return roughness < 0.12;
}

float MaterialFrostedGlass::DistributionGGX(glm::vec3 normal, glm::vec3 wh, float alphaSquare) {
    float dotNormalToWh = dot(normal, wh);
    float denom = Consts::PI * powf(dotNormalToWh * dotNormalToWh * (alphaSquare - 1.0f) + 1.0f, 2.0f);
    return alphaSquare / (denom);
}

float MaterialFrostedGlass::GeometrySchlickGGX(float dotNormalToW, float alphaSquare) {
    float cosThetaSquare = dotNormalToW * dotNormalToW;
    float sinThetaSquare = 1.0f - cosThetaSquare;
    float tanThetaSquare = sinThetaSquare / cosThetaSquare;
    float lamda = (std::sqrt(1.0f + alphaSquare * tanThetaSquare) - 1.0f) / 2.0f;
    return 1.0f / (1.0f + lamda);
}

float MaterialFrostedGlass::GeometrySmith(float absDotWiToNormal, float absDotWoToNormal, float alphaSquare) {
    float G1 = GeometrySchlickGGX(absDotWiToNormal, alphaSquare);
    float G2 = GeometrySchlickGGX(absDotWoToNormal, alphaSquare);
    return G1 * G2;
}

float MaterialFrostedGlass::FresnelSchlic(glm::vec3 wi, glm::vec3 wh) {
    float cosTheta = abs(dot(wh, wi));
    if (dot(wi, wh) > Consts::EPS) {    // 从里往外射
        glm::vec3 rayInLowIor = glm::refract(wi, -wh, mIor);
        if (glm::length(rayInLowIor) < Consts::EPS) {
            return 1.0f;
        }
        cosTheta = abs(dot(rayInLowIor, wh));
    }

    return mF0 + (1.0f - mF0) * powf(1.0f - cosTheta, 5.0f);
}

void MaterialFrostedGlass::GenerateKullaCountyMap() {
    // if (mEmuList.Width() * mEavgList.Height() != 0) {
    //     return;
    // }

    // LdsGenerator aGen(1, 2);
    // mEmuList = TexureSampler2D<float>(mEmuListSize, mEmuListSize);
    // mEavgList = TexureSampler2D<float>(1, mEmuListSize);

    // for (int i = 0; i < mEmuListSize; i++) {            // roughness
    //     for (int j = 0; j < mEmuListSize; j++) {        // theta
    //         // roughness
    //         float roughness = ((float)i + 0.5f) / mEmuListSize;
    //         float alpha = roughness * roughness;
    //         float alphaSquare = alpha * alpha;
        
    //         // cosTheta
    //         float dotWiToN = ((float)j + 0.5f) * 2.0f / mEmuListSize - 1.0f;       // (-1, 1)
    //         glm::vec3 wi = glm::vec3(std::sqrt(1.0f - dotWiToN * dotWiToN), 0.0f, dotWiToN);

    //         // integrate
    //         float e(0.0f);
    //         aGen.Reset(0, 0);
    //         aGen.Reset(1, 0);
    //         float sampleSize = mESample;
    //         for(int sample = 0; sample < mESample; sample++) {
    //             float ksi1 = aGen.Get(0, 0);
    //             float ksi2 = aGen.Get(1, 0);
    //             glm::vec3 wh = ImportanceSampleGGX(glm::vec2(ksi1, ksi2), Consts::Z_AXIS, alphaSquare);
                
    //             float fresnel = FresnelSchlic(wi, wh);
    //             float rand = Utils::GetUniformRandom();
    //             if (rand < fresnel) {
    //                 // reflect
    //                 glm::vec3 H = wh;    // 指向入射侧，专供refract使用
    //                 if (dotWiToN > 0) {
    //                     H = -H;
    //                 }
    //                 glm::vec3 wo = glm::reflect(wi, H);
    //                 float dotWoToN = glm::dot(wo, Consts::Z_AXIS);
    //                 if (dotWiToN * dotWoToN > 0.0f) {
    //                     sampleSize--;
    //                     continue;
    //                 }
    //                 float dotWiToH = glm::dot(wi, wh);
    //                 float dotWoToH = glm::dot(wo, wh);
    //                 float dotWhToN = std::max(wh.z, 0.0f);
    //                 // fr
    //                 float geometry = GeometrySmith(abs(dotWiToN), abs(dotWoToN), alphaSquare);
    //                 e += (geometry * dotWiToH) / (dotWhToN * dotWiToN);
    //             }
    //             else {
    //                 // refract
    //                 float etaI = 1.0f, etaT = mIor;
    //                 glm::vec3 H = wh;    // 指向入射侧，专供refract使用
    //                 if (dotWiToN > 0) {
    //                     std::swap(etaI, etaT);
    //                     H = -H;
    //                 }
    //                 float eta = etaI / etaT;
    //                 // wo
    //                 glm::vec3 wo = glm::refract(wi, H, eta);
    //                 if (glm::length(wo) < Consts::EPS) {
    //                     sampleSize--;
    //                     continue;
    //                 }
    //                 float dotWoToN = glm::dot(wo, Consts::Z_AXIS);
    //                 if (dotWiToN * dotWoToN < 0.0f) {
    //                     sampleSize--;
    //                     continue;
    //                 }
    //                 // fr
    //                 float dotWiToH = glm::dot(wi, H);
    //                 float dotWoToH = glm::dot(wo, H);
    //                 float geometry = GeometrySmith(abs(dotWiToN), abs(dotWoToN), alphaSquare);
    //                 e += geometry * abs(dotWiToH) / (abs(dotWiToN) * wh.z);
    //             }
    //         }
            
    //         e /= sampleSize;
    //         mEmuList.SetPixel(j, i, e);
    //     }
    // }
    // Utils::SaveImage(mEmuList.GetData(), "../../result/emufg.png", mEmuListSize, mEmuListSize, 1);

    // for (int i = 0; i < mEmuListSize; i++) {     // roughness
    //     float eAvg= 0.0f;
    //     for(int j = 0; j < mEmuListSize; j++) {  // 均匀采样 sinT
    //         float dotWiToN = ((float)j + 0.5f) / mEmuListSize; 
    //         eAvg += mEmuList.Sample(j, i) * dotWiToN * 2.0f / mEmuListSize;
    //     }
    //     mEavgList.SetPixel(0, i, eAvg);
    // }

    // Utils::SaveImage(mEavgList.GetData(), "../../result/eavgfg.png", 1, mEmuListSize, 1);
}

void MaterialFrostedGlass::SetRoughness(float roughness) {
    mRoughness = roughness;
    mAlpha = mRoughness * mRoughness;
    mAlphaSquare = mAlpha * mAlpha;
}

void MaterialFrostedGlass::SetRoughness(TexureSampler* roughnessTexure) {
    mRoughnessTexure = roughnessTexure;
}

void MaterialFrostedGlass::SetIor(float ior) {
    mIor = ior;
    mF0 = pow((1.0f - ior) / (1.0f + ior), 2);
}
