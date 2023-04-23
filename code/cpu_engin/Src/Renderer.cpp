#include "Renderer.h"
#include <thread>

namespace CpuEngin
{

Renderer::Renderer() {
    mLdsGenerator = new LdsGenerator();
}

Renderer::~Renderer() {
    if (mLdsGenerator != nullptr) {
        delete mLdsGenerator;
    }
}

void Renderer::Draw(float* image, Scene& scene) {

#ifdef SAVE_RAY
    Utils::mSaveRayPath = fopen("../../tools/SaveRay/Rays.txt", "wb");
#endif // SAVE_RAY

    mConfigInfo = scene.mConfigInfo;

    mInitTime = std::chrono::system_clock::now();
    mNumPixels = 0;

    // 初始化随机序列
    mLdsGenerator->Build(scene.mCamera.mHeight * scene.mCamera.mWidth, (mConfigInfo.maxTraceDepth + 1) * 2);

    // 开多线程
    std::vector<std::thread> threads;
    for (int i = 0; i < mConfigInfo.threadCount; i++) {
        int numRows = std::ceil((float)scene.mCamera.mHeight / (float)mConfigInfo.threadCount);
        int startRow = i * numRows;
        numRows = std::min(numRows, scene.mCamera.mHeight - startRow);
        // 创建线程
        threads.push_back(std::thread(&Renderer::DrawPart, this, std::ref(image), std::ref(scene), startRow, numRows, i));
    }

    // 等待线程结束
    for (int i = 0; i < mConfigInfo.threadCount; i++) {
        threads[i].join();
    }

    mNumPixels = 0;

    std::chrono::duration dur = std::chrono::system_clock::now() - mInitTime;
    auto hours = std::chrono::duration_cast<std::chrono::hours>(dur).count();
    auto minutes = std::chrono::duration_cast<std::chrono::minutes>(dur).count();
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(dur).count();
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();
    std::cout << std::endl;
    std::cout << "render time cost: " << hours % 60 << ":" << minutes % 60 << ":" << seconds % 60 << ":" << milliseconds % 1000 << std::endl;

#ifdef SAVE_RAY
    fclose(Utils::mSaveRayPath);
#endif // SAVE_RAY
}

void Renderer::DrawPart(float* framebuffer, Scene& scene, int startRow, int numRows, int threadNum) {
    // 相机参数
    float tanFovY_2 = tanf(Utils::DegToRad(scene.mCamera.mFovY) / 2.0f);
    float aspectRatio = float(scene.mCamera.mWidth) / float(scene.mCamera.mHeight);
    glm::mat4 viewInv = glm::inverse(scene.mCamera.mViewMatrix);
    float gama = 1.0f / scene.mCamera.mGama;

    float* p = framebuffer;
    p += startRow * scene.mCamera.mWidth * 3;

    int endRow = startRow + numRows;
    for (int i = startRow; i < endRow; i++)
    {
        for (int j = 0; j < scene.mCamera.mWidth; j++)
        {
            TraceInfo info;
            info.pixelNum = threadNum;//i * scene.mCamera.mWidth + j;
            info.saveRay = (i == Consts::SAVE_RAY_COORD.y) && (j == Consts::SAVE_RAY_COORD.x);
            glm::vec3 L(0.0f);
            for (int k = 0; k < mConfigInfo.spp; k++)
            {
                // 生成光线
                float dy = mLdsGenerator->Get(0, info.pixelNum) - 0.5f;
                float dx = mLdsGenerator->Get(1, info.pixelNum) - 0.5f;
                // float dy = Utils::GetUniformRandom(-0.5f, 0.5f);
                // float dx = Utils::GetUniformRandom(-0.5f, 0.5f);
                float y = -(2.0f * float(i + dy) / float(scene.mCamera.mHeight) - 1.0f) * tanFovY_2;
                float x = (2.0f * float(j + dx) / float(scene.mCamera.mWidth) - 1.0f) * tanFovY_2 * aspectRatio;

                glm::vec3 direction = glm::mat3(viewInv) * glm::normalize(glm::vec3(x, y, -1.0f));

                Ray ray;
                ray.origin = glm::vec3(viewInv[3]); // view矩阵第三列
                ray.direction = direction;
                
                // 投射光线
                glm::vec3 color = CastRay(scene, ray, info);
#ifdef CLAMP_COLOR
                color.x = std::clamp(color.x, 0.0f, 100.0f);
                color.y = std::clamp(color.y, 0.0f, 100.0f);
                color.z = std::clamp(color.z, 0.0f, 100.0f);
#endif // CLAMP_COLOR
                L += color;
            }
            L = L / float(mConfigInfo.spp);

            // 储存颜色+gama校正
            *p = powf(L.x, gama); p++;  // R 通道
            *p = powf(L.y, gama); p++;  // G 通道
            *p = powf(L.z, gama); p++;  // B 通道

            mNumPixels++;
        }

        mPrintMutex.lock();
        if (i % 10 == 0 || i == endRow - 1) {
            std::cout << 100.0f * float(mNumPixels) / float(scene.mCamera.mWidth * scene.mCamera.mHeight) << "% done \r" ;
            std::cout.flush();
        }
        mPrintMutex.unlock();
    }
}

glm::vec3 Renderer::CastRay(Scene& scene, Ray ray, TraceInfo info) {
    // 找交点并输出交点的颜色
    HitResult res = scene.GetIntersect(ray);
    //return glm::vec3(res.texCoord.y, res.texCoord.y, res.texCoord.y);
    //return glm::vec3((res.hitPoint.z + Consts::SCALE) / Consts::SCALE / 2.0f);
    glm::vec3 color(0.0f);
    if (res.isHit == 1) {
        if (res.material->HasEmission()) {
            // Lo = Le
            color = res.material->GetEmission();
        }
        else {
#ifdef SAVE_RAY
            if (info.saveRay) {
                fprintf(Utils::mSaveRayPath, "ray %.6f %.6f %.6f %.6f %.6f %.6f 0 0\n", ray.origin.x, ray.origin.y, ray.origin.z,
                    res.hitPoint.x, res.hitPoint.y, res.hitPoint.z);
            }
#endif // SAVE_RAY
            color = Shade(scene, ray, res, info);
        }
    }
    else {
        color = scene.mLightMap ? scene.mLightMap->Sample(ray.direction) : glm::vec3(0.0f);
    }

    return color;
}

glm::vec3 Renderer::Shade(Scene& scene, Ray ray, HitResult p, TraceInfo info) {
    info.depth++;
    if (info.depth > mConfigInfo.maxTraceDepth) {
        return glm::vec3(0.0f);
    }
#ifdef SAVE_RAY
    if (info.saveRay && info.depth < Consts::MAX_SAVE_DEPTH) {
        fprintf(Utils::mSaveRayPath, "norm %.6f %.6f %.6f %.6f %.6f %.6f %d\n", p.hitPoint.x, p.hitPoint.y, p.hitPoint.z,
            p.normal.x, p.normal.y, p.normal.z, info.depth);
    }
#endif // SAVE_RAY

    glm::vec3 Ldir(0.0f);
    glm::vec3 Lindir(0.0f);

    bool transparentSurface = p.material->GetType() == TRANSPARENT || 
                              p.material->GetType() == FROSTED_GLASS;

    // ==== 直接光照单独计算 ====
    if (scene.mNumIlluminant == 0) {
        Lindir = glm::vec3(0.0f);
    }
    else if (!p.material->IsExtremelySpecular(p.texCoord)) {
        float pdfLight;
        HitResult sampLightResult;
        scene.SampleLight(pdfLight, sampLightResult);
        glm::vec3 lightDir = sampLightResult.hitPoint - p.hitPoint;
        glm::vec3 lightDir_N = normalize(lightDir);

        float cosTheta = dot(lightDir_N, p.normal);
        float cosTheta_P = dot(-lightDir_N, sampLightResult.normal);

        Ray hitLightRay = { p.hitPoint, lightDir_N };
        HitResult hitLight = scene.GetIntersect(hitLightRay);
        if (hitLight.isHit == 1 && hitLight.distance < glm::length(lightDir) - Consts::EPS) {
            // 光源被遮挡
            Ldir = glm::vec3(0.0f);
        }
        else if (cosTheta_P < Consts::EPS/* || cosTheta < Consts::EPS*/) {
            // 光源背对物体 || 光源在物体背面
            Ldir = glm::vec3(0.0f);
        }
        else {
            SampleData evalData;
            evalData.normal = p.normal;
            evalData.wi = normalize(ray.direction);
            evalData.wo = lightDir_N;
            evalData.texCoord = p.texCoord;
            p.material->Eval(evalData);

            glm::vec3 Li = sampLightResult.material->GetEmission();
            float pntToLightSquare = dot(lightDir, lightDir);
            Ldir = Li * evalData.frCosine * abs(cosTheta) * cosTheta_P / (pntToLightSquare * pdfLight + Consts::EPS);
        }
    }
    else {
        // 绝对光滑表面另算
        Ray traceRay;
        traceRay.origin = p.hitPoint;

        SampleData directSampleData;
        directSampleData.normal = p.normal;
        directSampleData.wi = normalize(ray.direction);
        directSampleData.texCoord = p.texCoord;
        directSampleData.aRandomGenerator = mLdsGenerator;
        bool ret = p.material->SampleAndEval(directSampleData, info);
        traceRay.direction = directSampleData.wo;

        HitResult traceResult = scene.GetIntersect(traceRay);
        if (traceResult.isHit == 1 && traceResult.material->HasEmission() && ret) {
            Lindir = traceResult.material->GetEmission() * directSampleData.frCosine / (directSampleData.pdf + Consts::EPS);
        }
    }
    //return Ldir;

    // ==== 间接光照 ====
    float r = Utils::GetUniformRandom();
    float PRR = 0.8f;
    if (r < PRR) {
        // 构建下一条光线，并求fr pdf cosine
        Ray traceRay;
        traceRay.origin = p.hitPoint;

        SampleData indirectSampleData;
        indirectSampleData.normal = p.normal;
        indirectSampleData.wi = normalize(ray.direction);
        indirectSampleData.texCoord = p.texCoord;
        indirectSampleData.aRandomGenerator = mLdsGenerator;
        bool sampleSuccessFlag = p.material->SampleAndEval(indirectSampleData, info);
        traceRay.direction = indirectSampleData.wo;

        // 与场景求交
        HitResult traceResult = scene.GetIntersect(traceRay);
        bool traceDirectionFlag = transparentSurface || dot(p.normal, traceRay.direction) > Consts::EPS;    // 透明材质，或其他材质反射方向
        if (traceResult.isHit == 1 && !traceResult.material->HasEmission() && traceDirectionFlag) {
#ifdef SAVE_RAY
            if (info.saveRay && info.depth < Consts::MAX_SAVE_DEPTH) {
                fprintf(Utils::mSaveRayPath, "pnt %.6f %.6f %.6f %d\n", p.hitPoint.x, p.hitPoint.y, p.hitPoint.z, info.depth);
                fprintf(Utils::mSaveRayPath, "ray %.6f %.6f %.6f %.6f %.6f %.6f 0 %d\n", traceRay.origin.x, traceRay.origin.y, traceRay.origin.z,
                    traceResult.hitPoint.x, traceResult.hitPoint.y, traceResult.hitPoint.z, info.depth);
            }
#endif // SAVE_RAY
            Lindir = Shade(scene, traceRay, traceResult, info) * indirectSampleData.frCosine / (indirectSampleData.pdf * PRR + Consts::EPS);
        }
        else if (traceResult.isHit == 0 && scene.mLightMap != nullptr && traceDirectionFlag) {
            // 射到空气中
            Lindir = scene.mLightMap->Sample(traceRay.direction) * indirectSampleData.frCosine / (indirectSampleData.pdf * PRR + Consts::EPS);
        }
    }

    info.depth--;
    return Ldir + Lindir;
}

}
