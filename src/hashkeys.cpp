#include "color.h"
#include "square.h"
#include "publius.h"
#include "hashkeys.h"
#include <random>

std::mt19937_64 e2(2018);
std::uniform_int_distribution<Bitboard> dist(std::llround(std::pow(2, 56)), std::llround(std::pow(2, 62)));

void HashKeys::Init() {

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

Bitboard HashKeys::Random64(void) {
    Bitboard result = dist(e2);
    return result;
}