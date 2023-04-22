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

TexureSampler1F::TexureSampler1F(int w, int h) : TexureSampler(nullptr, w, h){
    mChannels = 1;
    mTexureData = new float[w * h * mChannels];
}

TexureSampler1F::~TexureSampler1F() {

}

glm::vec4 TexureSampler1F::Sample(glm::vec2 texCoord) {
	int i = std::clamp(int(texCoord.x * mWidth), 0, mWidth - 1);
	int j = std::clamp(int(texCoord.y * mHeight), 0, mHeight - 1);
	return glm::vec4(mTexureData[j * mWidth + i]);
}

// ===== TexureSampler2F =====
TexureSampler2F::TexureSampler2F(float* data, int w, int h) : TexureSampler(data, w, h) {
	mChannels = 2;
}

TexureSampler2F::TexureSampler2F(int w, int h) : TexureSampler(nullptr, w, h) {
    mChannels = 2;
    mTexureData = new float[w * h * mChannels];
}

TexureSampler2F::~TexureSampler2F() {

}

glm::vec4 TexureSampler2F::Sample(glm::vec2 texCoord) {
	int i = std::clamp(int(texCoord.x * mWidth), 0, mWidth - 1);
	int j = std::clamp(int(texCoord.y * mHeight), 0, mHeight - 1);
	float x = mTexureData[j * mWidth * mChannels + i * mChannels + 0];
	float y = mTexureData[j * mWidth * mChannels + i * mChannels + 1];
	return glm::vec4(x, y, 0.0f, 0.0f);
}

// ===== TexureSampler3F =====
TexureSampler3F::TexureSampler3F(float* data, int w, int h) : TexureSampler(data, w, h) {
	mChannels = 3;
}

TexureSampler3F::TexureSampler3F(int w, int h) : TexureSampler(nullptr, w, h) {
    mChannels = 3;
    mTexureData = new float[w * h * mChannels];
}

TexureSampler3F::~TexureSampler3F() {

}

glm::vec4 TexureSampler3F::Sample(glm::vec2 texCoord) {
	int i = std::clamp(int(texCoord.x * mWidth), 0, mWidth - 1);
	int j = std::clamp(int(texCoord.y * mHeight), 0, mHeight - 1);
	float x = mTexureData[j * mWidth * mChannels + i * mChannels + 0];
	float y = mTexureData[j * mWidth * mChannels + i * mChannels + 1];
	float z = mTexureData[j * mWidth * mChannels + i * mChannels + 2];

	return glm::vec4(x, y, z, 0.0f);
}

bool TexureSampler3F::SetPixel(glm::vec2 texCoord, glm::vec3 pixel) {
    int i = std::clamp(int(texCoord.x * mWidth), 0, mWidth - 1);
    int j = std::clamp(int(texCoord.y * mHeight), 0, mHeight - 1);
    mTexureData[j * mWidth * mChannels + i * mChannels + 0] = pixel.x;
    mTexureData[j * mWidth * mChannels + i * mChannels + 1] = pixel.y;
    mTexureData[j * mWidth * mChannels + i * mChannels + 2] = pixel.z;
    return true;
}

// ===== TexureSampler3F =====
TexureSampler4F::TexureSampler4F(float* data, int w, int h) : TexureSampler(data, w, h) {
	mChannels = 4;
}

TexureSampler4F::TexureSampler4F(int w, int h) : TexureSampler(nullptr, w, h){
    mChannels = 4;
    mTexureData = new float[w * h * mChannels];
}

TexureSampler4F::~TexureSampler4F() {

}

glm::vec4 TexureSampler4F::Sample(glm::vec2 texCoord) {
	int i = std::clamp(int(texCoord.x * mWidth), 0, mWidth - 1);
	int j = std::clamp(int(texCoord.y * mHeight), 0, mHeight - 1);
	float x = mTexureData[j * mWidth * mChannels + i * mChannels + 0];
	float y = mTexureData[j * mWidth * mChannels + i * mChannels + 1];
	float z = mTexureData[j * mWidth * mChannels + i * mChannels + 2];
	float w = mTexureData[j * mWidth * mChannels + i * mChannels + 3];
	return glm::vec4(x, y, z, w);
}

// ===== SphericalMap =====
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
