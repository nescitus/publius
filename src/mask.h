// Publius - Didactic public domain bitboard chess engine by Pawel Koziol

#pragma once

class MaskData {
private:
    void InitRanks();
    void InitPassedMask();
    void InitAdjacentMask();
    void InitSupportMask();
    void InitStrongPawn();
public:
    
    static constexpr int castle[64] = {
    13, 15, 15, 15, 12, 15, 15, 14,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    7,  15, 15, 15,  3, 15, 15, 11,
    };

    int kingAttack[256];
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