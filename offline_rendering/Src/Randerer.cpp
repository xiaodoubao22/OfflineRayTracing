#include "Randerer.h"
#include <thread>


void Randerer::Draw(float* image, Scene& scene) {
#ifdef SAVE_RAY
    Utils::mSaveRayPath = fopen("../scripts/SaveRay/Rays.txt", "wb");
#endif // SAVE_RAY

    mInitTime = std::chrono::system_clock::now();
    mNumPixels = 0;
    
    // 开多线程
    std::vector<std::thread> threads;
    for (int i = 0; i < Consts::THREAD_COUNT; i++) {
        int numRows = std::ceil((float)Consts::HEIGHT / (float)Consts::THREAD_COUNT);
        int startRow = i * numRows;
        numRows = std::min(numRows, Consts::HEIGHT - startRow);
        // 创建线程
        threads.push_back(std::thread(&Randerer::DrawPart, this, std::ref(image), std::ref(scene), startRow, numRows));
    }

    // 等待线程结束
    for (int i = 0; i < Consts::THREAD_COUNT; i++) {
        threads[i].join();
    }

    mNumPixels = 0;

    std::chrono::duration dur = std::chrono::system_clock::now() - mInitTime;
    auto hours = std::chrono::duration_cast<std::chrono::hours>(dur).count();
    auto minutes = std::chrono::duration_cast<std::chrono::minutes>(dur).count();
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(dur).count();
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();
    std::cout << std::endl;
    std::cout << "rander time cost: " << hours % 60 << ":" << minutes % 60 << ":" << seconds % 60 << ":" << milliseconds % 1000 << std::endl;

#ifdef SAVE_RAY
    fclose(Utils::mSaveRayPath);
#endif // SAVE_RAY

}

void Randerer::DrawPart(float* framebuffer, Scene& scene, int startRow, int numRows) {
    // 相机参数
    float tanFovY_2 = tanf(Utils::DegToRad(Consts::FOVY) / 2.0f);
    float aspectRatio = float(Consts::WIDTH) / float(Consts::HEIGHT);

    float* p = framebuffer;
    p += startRow * Consts::WIDTH * 3;

    int endRow = startRow + numRows;
    for (int i = startRow; i < endRow; i++)
    {
        for (int j = 0; j < Consts::WIDTH; j++)
        {
            bool saveRayFlag = i == Consts::SAVE_RAY_COORD.y && j == Consts::SAVE_RAY_COORD.x;
            glm::vec3 L(0.0f);
            for (int k = 0; k < Consts::SPP; k++)
            {
                // 生成光线
                float dy = Utils::GetUniformRandom(-0.5f, 0.5f);
                float dx = Utils::GetUniformRandom(-0.5f, 0.5f);
                float y = -(2.0f * float(i + dy) / float(Consts::HEIGHT) - 1.0f) * tanFovY_2;
                float x = (2.0f * float(j + dx) / float(Consts::WIDTH) - 1.0f) * tanFovY_2 * aspectRatio;

                glm::vec3 direction(x, y, -1.0f);
                direction = glm::normalize(direction);

                Ray ray;
                ray.origin = Consts::EYE;
                ray.direction = direction;
                
                // 投射光线
                glm::vec3 color = CastRay(scene, ray, saveRayFlag);
#ifdef CLAMP_COLOR
                color.x = std::clamp(color.x, 0.0f, 100.0f);
                color.y = std::clamp(color.y, 0.0f, 100.0f);
                color.z = std::clamp(color.z, 0.0f, 100.0f);
#endif // CLAMP_COLOR
                L += color;
            }
            L = L / float(Consts::SPP);

            *p += L.x; p++;  // R 通道
            *p += L.y; p++;  // G 通道
            *p += L.z; p++;  // B 通道

            mNumPixels++;

        }

        mPrintMutex.lock();
        if (i % 10 == 0 || i == endRow - 1) {
            std::cout << 100.0f * float(mNumPixels) / float(Consts::WIDTH * Consts::HEIGHT) << "% done \r" ;
            std::cout.flush();
        }
        mPrintMutex.unlock();
    }
}

glm::vec3 Randerer::CastRay(Scene& scene, Ray ray, bool saveRay) {
    // 找交点并输出交点的颜色
    HitResult res = scene.GetIntersect(ray);
    //return glm::vec3((res.hitPoint.z + Consts::SCALE) / Consts::SCALE / 2.0f);
    glm::vec3 color(0.0f);
    if (res.isHit == 1) {
        if (res.material->HasEmission()) {
            // Lo = Le
            color = res.material->GetEmission();
        }
        else {
#ifdef SAVE_RAY
            if (saveRay) {
                fprintf(Utils::mSaveRayPath, "ray %.6f %.6f %.6f %.6f %.6f %.6f 0 0\n", ray.origin.x, ray.origin.y, ray.origin.z,
                    res.hitPoint.x, res.hitPoint.y, res.hitPoint.z);
            }
#endif // SAVE_RAY
            color = Shade(scene, ray, res, 0, saveRay);
        }
    }

    return color;
}

glm::vec3 Randerer::Shade(Scene& scene, Ray ray, HitResult p, int depth, bool saveRay) {
    depth++;
    glm::vec3 Ldir(0.0f);
    glm::vec3 Lindir(0.0f);

    bool specularSurface = p.material->GetType() == SPECULAR || 
                           p.material->GetType() == TRANSPARENT;
    bool transparentSurface = p.material->GetType() == TRANSPARENT || 
                              p.material->GetType() == FROSTED_GLASS;

    // ====直接光照单独计算====
    if (!specularSurface) {
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
            glm::vec3 fr = p.material->Eval(p.normal, normalize(ray.direction), lightDir_N);
            glm::vec3 Li = sampLightResult.material->GetEmission();
            float pntToLightSquare = dot(lightDir, lightDir);
            Ldir = Li * fr * abs(cosTheta) * cosTheta_P / (pntToLightSquare * pdfLight + Consts::EPS);
        }
    }
    else {
        // 绝对光滑表面另算
        Ray traceRay;
        traceRay.origin = p.hitPoint;
        float pdf;
        glm::vec3 fr;
        bool ret = p.material->SampleAndEval(p.normal, normalize(ray.direction), traceRay.direction, pdf, fr);
        HitResult traceResult = scene.GetIntersect(traceRay);
        if (traceResult.isHit == 1 && traceResult.material->HasEmission() && ret) {
            Lindir = traceResult.material->GetEmission() * fr / (pdf + Consts::EPS);
        }
    }
    //return Ldir;

    // ====间接光照====
    float r = Utils::GetUniformRandom();
    float PRR = 0.8f;
    if (r < PRR) {
        // 构建下一条光线，并求fr pdf cosine
        Ray traceRay;
        traceRay.origin = p.hitPoint;
        float pdf;
        glm::vec3 fr;
#ifdef SAVE_RAY
        if (saveRay && depth < Consts::MAX_SAVE_DEPTH) {
            fprintf(Utils::mSaveRayPath, "norm %.6f %.6f %.6f %.6f %.6f %.6f %d\n", p.hitPoint.x, p.hitPoint.y, p.hitPoint.z,
                p.normal.x, p.normal.y, p.normal.z, depth);
        }
#endif // SAVE_RAY
        bool ret = p.material->SampleAndEval(p.normal, ray.direction, traceRay.direction, pdf, fr);

        // 与场景求交
        HitResult traceResult = scene.GetIntersect(traceRay);

        if (traceResult.isHit == 1 && !traceResult.material->HasEmission() && ret) {
            if (transparentSurface || dot(p.normal, traceRay.direction) > Consts::EPS) {
#ifdef SAVE_RAY
                if (saveRay && depth < Consts::MAX_SAVE_DEPTH) {
                    fprintf(Utils::mSaveRayPath, "pnt %.6f %.6f %.6f %d\n", p.hitPoint.x, p.hitPoint.y, p.hitPoint.z, depth);
                    fprintf(Utils::mSaveRayPath, "ray %.6f %.6f %.6f %.6f %.6f %.6f 0 %d\n", traceRay.origin.x, traceRay.origin.y, traceRay.origin.z,
                        traceResult.hitPoint.x, traceResult.hitPoint.y, traceResult.hitPoint.z, depth);
                }
#endif // SAVE_RAY
                Lindir = Shade(scene, traceRay, traceResult, depth, saveRay) * fr / (pdf * PRR + Consts::EPS);
            }
        }
    }

    depth--;
    return Ldir + Lindir;
}
