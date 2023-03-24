#ifndef TEXURE_SAMPLER
#define TEXURE_SAMPLER

#include "Utils.h"

class TexureSampler
{
protected:
	float* mTexureData = nullptr;
	int mWidth;
	int mHeight;

public:
	TexureSampler() = delete;
	TexureSampler(float* data, int w, int h);
	virtual ~TexureSampler();

	//virtual glm::vec4 Sample(glm::vec2 texCoord) = 0;
};

// ===== TexureSampler1F =====
class TexureSampler1F : public TexureSampler
{
public:
	TexureSampler1F(float* data, int w, int h);
	virtual ~TexureSampler1F();
	float Sample(glm::vec2 texCoord);
};

// ===== TexureSampler2F =====
class TexureSampler2F : public TexureSampler
{
private:
	const int mChannels = 2;
public:
	TexureSampler2F(float* data, int w, int h);
	virtual ~TexureSampler2F();
	glm::vec2 Sample(glm::vec2 texCoord);
};

// ===== TexureSampler3F =====
class TexureSampler3F : public TexureSampler
{
private:
	const int mChannels = 3;
public:
	TexureSampler3F(float* data, int w, int h);
	virtual ~TexureSampler3F();
	glm::vec3 Sample(glm::vec2 texCoord);
};

class SphericalMap 
{
private:
	TexureSampler3F mLightMap;
	bool mIsEmpty = false;

public:
	SphericalMap(float* data, int w, int h);

	glm::vec3 Sample(glm::vec3 direction);
};


#endif // !TEXURE_SAMPLER
