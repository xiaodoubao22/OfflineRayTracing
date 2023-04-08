#include "TexureSampler.h"

// ===== TexureSampler =====
TexureSampler::TexureSampler(float* data, int w, int h) {
	mTexureData = data;
	mWidth = w;
	mHeight = h;
}

TexureSampler::~TexureSampler() {
	if (mTexureData != nullptr) {
		delete mTexureData;
	}	
}

int TexureSampler::NumChannels() {
	return mChannels;
}

// ===== TexureSampler1F =====
TexureSampler1F::TexureSampler1F(float* data, int w, int h) : TexureSampler(data, w, h) {
	mChannels = 1;
}

TexureSampler1F::~TexureSampler1F() {

}

float TexureSampler1F::Sample(glm::vec2 texCoord) {
	int i = std::clamp(int(texCoord.x * mWidth), 0, mWidth - 1);
	int j = std::clamp(int(texCoord.y * mHeight), 0, mHeight - 1);
	return mTexureData[j * mWidth + i];
}

// ===== TexureSampler2F =====
TexureSampler2F::TexureSampler2F(float* data, int w, int h) : TexureSampler(data, w, h) {
	mChannels = 2;
}

TexureSampler2F::~TexureSampler2F() {

}

glm::vec2 TexureSampler2F::Sample(glm::vec2 texCoord) {
	int i = std::clamp(int(texCoord.x * mWidth), 0, mWidth - 1);
	int j = std::clamp(int(texCoord.y * mHeight), 0, mHeight - 1);
	float x = mTexureData[j * mWidth * mChannels + i * mChannels + 0];
	float y = mTexureData[j * mWidth * mChannels + i * mChannels + 1];
	return glm::vec2(x, y);
}

// ===== TexureSampler3F =====
TexureSampler3F::TexureSampler3F(float* data, int w, int h) : TexureSampler(data, w, h) {
	mChannels = 3;
}

TexureSampler3F::~TexureSampler3F() {

}

glm::vec3 TexureSampler3F::Sample(glm::vec2 texCoord) {
	int i = std::clamp(int(texCoord.x * mWidth), 0, mWidth - 1);
	int j = std::clamp(int(texCoord.y * mHeight), 0, mHeight - 1);
	float x = mTexureData[j * mWidth * mChannels + i * mChannels + 0];
	float y = mTexureData[j * mWidth * mChannels + i * mChannels + 1];
	float z = mTexureData[j * mWidth * mChannels + i * mChannels + 2];

	return glm::vec3(x, y, z);
}

SphericalMap::SphericalMap(float* data, int w, int h) : mLightMap(data, w, h) {
	if (data == nullptr) mIsEmpty = true;
}

glm::vec3 SphericalMap::Sample(glm::vec3 direction) {
	if (mIsEmpty) return glm::vec3(0.0f, 0.0f, 0.0f);

	float pitch = asinf(direction.z);
	float yaw = -atan2f(direction.y, direction.x);

	float texCoordX = 0.5f * yaw / Consts::PI + 0.5f;
	float texCoordY = pitch / Consts::PI + 0.5f;

	return mLightMap.Sample(glm::vec2(texCoordX, texCoordY));
}
