#include "LdsGenerator.h"

LdsGenerator::LdsGenerator() {

}

LdsGenerator::LdsGenerator(int pixelNum, int dim) {
    Build(pixelNum, dim);
}

void LdsGenerator::Build(int pixelNum, int dim) {
    uint32_t bit = 1;
    mBits = std::vector<uint32_t>(32);
    for (int i = 0; i < 32; i++) {
        mBits[i] = bit;
        bit = bit << 1;
    }

    int idxOffs = 1;
    mDirNumV_.resize(dim);

    // 计算第0个维度所有m都为1
    std::vector<uint32_t> arrV(32);
    for (int k = 1; k <= 32; k++) {
        arrV[k - idxOffs] = 1u << (32 - k);
    }
    mDirNumV_[0] = arrV;
    
    // 计算其余维度
    for (int d = 1; d < dim; d++) {
        int sj = mMSet[d].size();
        // 补全m
        std::vector<uint32_t> arrM = mMSet[d];
        arrM.resize(32);

        for (int k = sj + 1; k <= 32; k++) {        // k~[sj + 1,32]
            uint32_t mk = arrM[k - sj - idxOffs];            // 最后一项m[k-sj]
            mk ^= ((1u << sj) * arrM[k - sj - idxOffs]);    // 倒数第二项(2^sj)*m[k-sj]
            for (int i = 1; i < sj; i++) {        // i~[1, sj-1]i - idxOffs
                mk ^= ((1u << i) * arrM[k - i - idxOffs] * (mASet[d] & mBits[sj - i - idxOffs]));    // 其他项
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

    // 初始化生成索引和记录
    mIndex.resize(dim);
    mDigit32.resize(dim);
    for (int i = 0; i < dim; i++) {
        mIndex[i] = std::vector<uint32_t>(pixelNum, 0);
        mDigit32[i] = std::vector<uint32_t>(pixelNum, 0);
    }
}

float LdsGenerator::Get(int dim, int pixelNum) {
    int idxOffs = 1;
    uint32_t i = mIndex[dim][pixelNum];
    uint32_t ci = 1u;   // 找到右起第一个是0的位
    while ((i & 1u) != 0u) {
        i = i >> 1u;
        ci++;
    }
    mIndex[dim][pixelNum]++;
    mDigit32[dim][pixelNum] = (mDigit32[dim][pixelNum] ^ mDirNumV_[dim][ci - idxOffs]);
    return float(mDigit32[dim][pixelNum]) * mFactor;
}

void LdsGenerator::Reset() {

}

void LdsGenerator::Reset(int dim, int thread) {
    mIndex[dim][thread] = 0;
    mDigit32[dim][thread] = 0;
}
