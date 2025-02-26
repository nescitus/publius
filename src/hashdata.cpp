#include "color.h"
#include "square.h"
#include "publius.h"
#include "bitboard.h"
#include "hashdata.h"

void HashData::Init() {

    for (int piece = 0; piece < 12; piece++)
        for (Square square = A1; square < sqNone; ++square)
            pieceKey[piece][square] = Random64();


    for (int i = 0; i < 16; i++) {
        castleKey[i] = Random64();
    }

    for (int i = 0; i < 8; i++) {
        enPassantKey[i] = Random64();
    }
}