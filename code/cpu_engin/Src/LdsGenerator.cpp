#include "LdsGenerator.h"

LdsGenerator::LdsGenerator() {
}

LdsGenerator* LdsGenerator::GetInstance() {
	static LdsGenerator instance;
	return &instance;
}

void LdsGenerator::Build(int threadCount, int dim, std::vector<uint32_t>& a, std::vector<std::vector<uint32_t>>& m) {
	uint32_t bit = 1;
	mBits = std::vector<uint32_t>(32);
	for (int i = 0; i < 32; i++) {
		mBits[i] = bit;
		bit = bit << 1;
	}

	mIndex.resize(dim);
	mDigit32.resize(dim);
	for (int i = 0; i < dim; i++) {
		mIndex[i] = std::vector<uint32_t>(threadCount);
		mDigit32[i] = std::vector<uint32_t>(threadCount);
	}

	int idxOffs = 1;
	mDirNumV_.resize(dim);
	for (int d = 0; d < dim; d++) {
		int sj = m[d].size();
		// 补全m
		std::vector<uint32_t> arrM = m[d];
		arrM.resize(32);

		for (int k = sj + 1; k <= 32; k++) {		// k~[1,32]
			uint32_t mk = arrM[k - sj - idxOffs];			// 最后一项
			mk ^= ((1ull << sj) * arrM[k - sj - idxOffs]);	// 倒数第二项
			for (int i = 1; i < sj; i++) {		// i~[1, sj-1]i - idxOffs
				mk ^= ((1ull << i) * arrM[k - i - idxOffs] * (a[d] & mBits[sj - i - idxOffs]));	// 其他项
			}
			arrM[k - idxOffs] = mk;
		}
		// 求v
		std::vector<uint32_t> arrV(32);
		for (int k = 1; k <= 32; k++) {
			arrV[k - idxOffs] = arrM[k - idxOffs] << (32 - k);
		}
		mDirNumV_[d] = arrV;
	}
}

float LdsGenerator::Get(int dim, int thread) {
	uint32_t i = mIndex[dim][thread];
	uint32_t ci = 1u;
	while ((i & 1u) != 0u) {
		i = i >> 1u;
		ci++;
	}
	mIndex[dim][thread]++;
	mDigit32[dim][thread] = (mDigit32[dim][thread] ^ mDirNumV_[dim][ci - 1]) * bool(mIndex[dim][thread]);
	return float(mDigit32[dim][thread]) * mFactor;
}

void LdsGenerator::Reset() {

}

void LdsGenerator::Reset(int dim, int thread) {
	mIndex[dim][thread] = 0;
	mDigit32[dim][thread] = 0;
}
