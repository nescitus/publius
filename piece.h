// Publius - Didactic public domain bitboard chess engine 
// by Pawel Koziol

// decoding and encoding piece

#pragma once

// given a piece, return its color
constexpr inline Color ColorOfPiece(ColoredPiece piece) {
    return (Color)(piece & 1);
}

// given a piece, return its type
constexpr inline PieceType TypeOfPiece(ColoredPiece piece) {
    return (PieceType)(piece >> 1);
}

// get unique id of a piece (denoting, say, white knight), range 0-11
constexpr inline ColoredPiece CreatePiece(Color pieceColor, PieceType pieceType) {
    return (ColoredPiece)((pieceType << 1) | pieceColor);
}
 