// Publius - Didactic public domain bitboard chess engine 
// by Pawel Koziol

#include "types.h"
#include "piece.h"
#include "hashkeys.h"
#include <random>

std::mt19937_64 e2(2018);
std::uniform_int_distribution<Bitboard> dist(std::llround(std::pow(2, 56)), std::llround(std::pow(2, 62)));

HashKeys::HashKeys() {

    // init piece/square keys
    for (ColoredPiece piece = WhitePawn; piece < noPiece; ++piece)
        for (Square square = A1; square < sqNone; ++square)
            pieceKey[piece][square] = Random64();

    // init castle keys
    for (int i = 0; i < 16; i++)
        castleKey[i] = Random64();

    // init en passant keys
    for (int i = 0; i < 8; i++)
        enPassantKey[i] = Random64();
}

Bitboard HashKeys::Random64(void) {
    return dist(e2);
}

Bitboard HashKeys::ForPiece(Color color, PieceType pieceType, Square square) {
    return pieceKey[CreatePiece(color, pieceType)][square];
}