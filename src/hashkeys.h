#pragma once

static constexpr auto sideRandom = ~((Bitboard)0);

class HashKeys {
public:
    HashKeys(); // constructor
    Bitboard Random64(void);
    Bitboard ForPiece(Color color, int pieceType, Square square);
    Bitboard pieceKey[12][64];
    Bitboard castleKey[16];
    Bitboard enPassantKey[8];
};

extern HashKeys Key;