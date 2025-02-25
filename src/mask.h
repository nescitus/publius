#pragma once

// mask

class cMask {
private:
    void InitHashKeys();
    void InitRanks();
    void InitPassedMask();
    void InitAdjacentMask();
    void InitSupportMask();
public:
    Bitboard pieceKey[12][64];
    Bitboard castleKey[16];
    Bitboard enPassantKey[8];
    Bitboard rank[8];
    Bitboard file[8];
    Bitboard passed[2][64];
    Bitboard support[2][64];
    Bitboard adjacent[8];
    void Init();
};

extern cMask Mask;