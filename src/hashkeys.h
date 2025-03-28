#pragma once

static constexpr auto sideRandom = ~((Bitboard)0);

class HashKeys {
public:
    void Init();
    Bitboard Random64(void);
    Bitboard pieceKey[12][64];
    Bitboard castleKey[16];
    Bitboard enPassantKey[8];
};

extern HashKeys Key;