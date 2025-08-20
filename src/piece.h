// Publius - Didactic public domain bitboard chess engine 
// by Pawel Koziol

// decoding and encoding piece

#pragma once

// given a piece, return its color
constexpr inline Color ColorOfPiece(int piece) {
    return (Color)(piece & 1);
}

// given a piece, return its type
constexpr inline int TypeOfPiece(int piece) {
    return piece >> 1;
}

// get unique id of a piece (denoting, say, white knight), range 0-11
constexpr inline  int CreatePiece(Color pieceColor, int pieceType) {
    return (pieceType << 1) | pieceColor;
}
 