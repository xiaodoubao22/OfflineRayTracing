#ifndef RANDERER_H
#define RANDERER_H

#include "Utils.h"
#include "Scene.h"
#include <chrono>
#include <mutex>

class Randerer {
public:
	void Draw(float* image, Scene& scene);

private:
	void DrawPart(float* framebuffer, Scene& scene, int startRow, int numRow);
	glm::vec3 CastRay(Scene& scene, Ray ray, bool saveRay = false);
	glm::vec3 Shade(Scene& scene, Ray ray, HitResult p, int depth, bool saveRay = false);

private:
	std::chrono::system_clock::time_point mInitTime;
	std::chrono::system_clock::time_point mCurrentTime;
	std::mutex mPrintMutex;
	int mNumPixels;

};

#endif // !RANDERER_H

