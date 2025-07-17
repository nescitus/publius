// Publius - Didactic public domain bitboard chess engine by Pawel Koziol

#include "types.h"
#include "piece.h"

Color ColorOfPiece(int piece) {
    return (Color)(piece & 1);
}

int TypeOfPiece(int piece) {
    return piece >> 1;
}

int CreatePiece(Color pieceColor, int pieceType) {
    return (pieceType << 1) | pieceColor;
}