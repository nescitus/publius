#pragma once

// mask

class MaskData {
private:
    void InitRanks();
    void InitPassedMask();
    void InitAdjacentMask();
    void InitSupportMask();
public:
    Bitboard rank[8];
    Bitboard file[8];
    Bitboard rr[2][8];
    Bitboard passed[2][64];
    Bitboard support[2][64];
    Bitboard adjacent[8];
    void Init();
};

extern MaskData Mask;