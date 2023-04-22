#ifndef RENDERER_H
#define RENDERER_H

#include "Utils.h"
#include "Scene.h"
#include "LdsGenerator.h"
#include <chrono>
#include <mutex>

namespace CpuEngin
{

class Renderer {
public:
	Renderer();
    ~Renderer();
	void Draw(float* image, Scene& scene);

private:
	void DrawPart(float* framebuffer, Scene& scene, int startRow, int numRow, int threadNum);
	glm::vec3 CastRay(Scene& scene, Ray ray, TraceInfo info);
	glm::vec3 Shade(Scene& scene, Ray ray, HitResult p, TraceInfo info);

private:
	std::chrono::system_clock::time_point mInitTime;
	std::chrono::system_clock::time_point mCurrentTime;
	std::mutex mPrintMutex;
	int mNumPixels = 0;
	LdsGenerator* mLdsGenerator = nullptr;
	CpuEnginConfig mConfigInfo;

};

#endif // !RENDERER_H

}
