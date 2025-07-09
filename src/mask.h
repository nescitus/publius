#pragma once

// mask

class MaskData {
private:
    void InitRanks();
    void InitPassedMask();
    void InitAdjacentMask();
    void InitSupportMask();
    void InitStrongPawn();
public:
    Bitboard rank[8];
    Bitboard file[8];
    Bitboard rr[2][8]; // relative rank
    Bitboard passed[2][64];
    Bitboard strongPawn[2][64];
    Bitboard support[2][64];
    Bitboard adjacentFiles[8];
    MaskData(); // constructor
};

extern MaskData Mask;