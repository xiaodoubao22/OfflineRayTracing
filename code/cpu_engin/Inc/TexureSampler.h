#ifndef TEXURE_SAMPLER
#define TEXURE_SAMPLER

#include "Utils.h"

class TexureSampler
{
public:
	float* mTexureData = nullptr;
	int mWidth;
	int mHeight;
	int mChannels = 0;

public:
	TexureSampler() = delete;
	TexureSampler(float* data, int w, int h);
	virtual ~TexureSampler();

	int NumChannels();
	virtual glm::vec4 Sample(glm::vec2 texCoord) = 0;
	//virtual glm::vec4 Sample(glm::vec2 texCoord) = 0;
};

// ===== TexureSampler1F =====
class TexureSampler1F : public TexureSampler
{
public:
	TexureSampler1F(float* data, int w, int h);
    TexureSampler1F(int w, int h);
	virtual ~TexureSampler1F();
	virtual glm::vec4 Sample(glm::vec2 texCoord) override;
};

// ===== TexureSampler2F =====
class TexureSampler2F : public TexureSampler
{
public:
	TexureSampler2F(float* data, int w, int h);
    TexureSampler2F(int w, int h);
	virtual ~TexureSampler2F();
	virtual glm::vec4 Sample(glm::vec2 texCoord) override;
};

// ===== TexureSampler3F =====
class TexureSampler3F : public TexureSampler
{
public:
	TexureSampler3F(float* data, int w, int h);
    TexureSampler3F(int w, int h);
	virtual ~TexureSampler3F();
	virtual glm::vec4 Sample(glm::vec2 texCoord) override;
    bool SetPixel(glm::vec2 texCoord, glm::vec3 pixel);
};

// ===== TexureSampler3F =====
class TexureSampler4F : public TexureSampler
{
public:
	TexureSampler4F(float* data, int w, int h);
    TexureSampler4F(int w, int h);
	virtual ~TexureSampler4F();
	virtual glm::vec4 Sample(glm::vec2 texCoord) override;
};


template <typename T>
class TexureSampler2D
{
private:
    std::vector<T> mData;
    int mWidth = 0;
    int mHeight = 0;

public:
	TexureSampler2D() {

    }

    TexureSampler2D(int w, int h) {
        mWidth = w;
        mHeight = h;
        mData.resize(w * h);
    }

    ~TexureSampler2D() {

    }

public:
	bool SetData(void* srcData, int w, int h) {
        int srcDataSize = std::min(w * h * sizeof(T), mData.size() * sizeof(T));
        int ret = memcpy_s(mData.data(), mData.size() * sizeof(T), srcData, srcDataSize);
        return ret == 0 ? true : false;
    }

    T* GetData() {
        return mData.data();
    }

	T Sample(float u, float v) {
        int row = std::clamp(int(v * mHeight), 0, mHeight - 1);
        int col = std::clamp(int(u * mWidth), 0, mWidth - 1);
        return mData[row * mWidth + col];
    }

    T Sample(int c, int r) {
        int row = std::clamp(r, 0, mHeight - 1);
        int col = std::clamp(c, 0, mWidth - 1);
        return mData[row * mWidth + col];
    }

    bool SetPixel(float u, float v, T pixel) {
        int row = std::clamp(int(v * mHeight), 0, mHeight - 1);
        int col = std::clamp(int(u * mWidth), 0, mWidth - 1);
        mData[row * mWidth + col] = pixel;
        return true;
    }

    bool SetPixel(int c, int r, T pixel) {
        int row = std::clamp(r, 0, mHeight - 1);
        int col = std::clamp(c, 0, mWidth - 1);
        mData[row * mWidth + col] = pixel;
        return true;
    }

    int Width() {
        return mWidth;
    }

    int Height() {
        return mHeight;
    }
};

// ===== SphericalMap =====
class SphericalMap 
{
private:
	TexureSampler3F mLightMap;
	bool mIsEmpty = false;
	float mPitchOffset = 0.0f;
	float mYawOffset = 0.0f;

public:
	SphericalMap(float* data, int w, int h);

	glm::vec3 Sample(glm::vec3 direction);
};


#endif // !TEXURE_SAMPLER
