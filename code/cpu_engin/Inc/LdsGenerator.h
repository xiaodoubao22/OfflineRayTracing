#ifndef LDS_GENERATOR
#define LDS_GENERATOR

#include "Utils.h"

class LdsGenerator 
{
public:
	LdsGenerator();
	static LdsGenerator* GetInstance();
	void Build(int threadCount, int dim, std::vector<uint32_t>& a, std::vector<std::vector<uint32_t>>& m);
	float Get(int dim, int thread);
	void Reset();
	void Reset(int dim, int thread);
	bool show = false;
private:
	std::vector<std::vector<uint32_t>> mDirNumV_;	// [dim][k]
	std::vector<std::vector<uint32_t>> mIndex;		// [dim][thread]
	std::vector<std::vector<uint32_t>> mDigit32;	// [dim][thread]
	std::vector<uint32_t> mBits;
	float mFactor = 1.0f / (1ull << 32);
};


#endif // !LDS_GENERATOR

