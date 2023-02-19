#ifndef RANDERER_H
#define RANDERER_H

#include "Utils.h"
#include "Scene.h"
#include "LdsGenerator.h"
#include <chrono>
#include <mutex>

class Randerer {
public:
	Randerer();
	void Draw(float* image, Scene& scene);

private:
	void DrawPart(float* framebuffer, Scene& scene, int startRow, int numRow, int threadNum);
	glm::vec3 CastRay(Scene& scene, Ray ray, TraceInfo info);
	glm::vec3 Shade(Scene& scene, Ray ray, HitResult p, TraceInfo info);

private:
	std::chrono::system_clock::time_point mInitTime;
	std::chrono::system_clock::time_point mCurrentTime;
	std::mutex mPrintMutex;
	int mNumPixels;
	LdsGenerator* mLdsGenerator;

};

#endif // !RANDERER_H

