#pragma once

class HashData {
public:
    void Init();
    Bitboard Random64(void);
    Bitboard pieceKey[12][64];
    Bitboard castleKey[16];
    Bitboard enPassantKey[8];
};

extern HashData Key;