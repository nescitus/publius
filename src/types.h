// Publius - Didactic public domain bitboard chess engine by Pawel Koziol

#pragma once

typedef unsigned long long Bitboard;
typedef int Move;

/* Define Color enum and what can be done with it.

The engine would work just fine using int instead 
of Color, but it is better to introduce some type-safety.
You will get a compiler error when attempting to use Color
instead of Square or trying to add two Color variables
together. In fact, there are only two things you can do
with Color: flip it so that White becomes Black
or another way round using ~operator or use ++ operator
to traverse the loop and do something both with white
and black pieces. */

enum Color { White = 0, Black = 1, colorNone = 2 };
static_assert(White == 0 && Black == 1, "Color encoding required by ~ operator");

// switch color
constexpr inline Color operator~(Color c) {
    return Color(c ^ Black);
}

// increment color in a "for" loop
constexpr inline Color operator++(Color& d) {
    return d = Color(int(d) + 1);
}

enum PieceType { Pawn, Knight, Bishop, Rook, Queen, King, noPieceType };

enum eFile { fileA, fileB, fileC, fileD, fileE, fileF, fileG, fileH };
enum eRank { rank1, rank2, rank3, rank4, rank5, rank6, rank7, rank8 };

// Define Square enum and what can be done with it

enum Square {
    A1, B1, C1, D1, E1, F1, G1, H1,
    A2, B2, C2, D2, E2, F2, G2, H2,
    A3, B3, C3, D3, E3, F3, G3, H3,
    A4, B4, C4, D4, E4, F4, G4, H4,
    A5, B5, C5, D5, E5, F5, G5, H5,
    A6, B6, C6, D6, E6, F6, G6, H6,
    A7, B7, C7, D7, E7, F7, G7, H7,
    A8, B8, C8, D8, E8, F8, G8, H8, sqNone
};

// There are only so many things you may want
// to do with squares and many things that
// make no sense, like multiplying E2 * E4.
// That's why our Square type comes with
// a narrow set of legal operations:

// step through squares, usually in a loop
constexpr inline Square operator++(Square& d) {
    return d = Square(int(d) + 1);
}

// add vector to a square
constexpr inline Square operator+(Square d1, int d2) {
    return Square(int(d1) + int(d2));
}

// substract vector from a square
constexpr inline Square operator-(Square d1, int d2) {
    return Square(int(d1) - int(d2));
}

// needed for en passant
constexpr inline Square operator^(Square d1, int d2) {
    return Square(int(d1) ^ d2);
}

enum eMoveType { tNormal, tCastle, tEnPassant, tPawnjump, tPromN, tPromB, tPromR, tPromQ };
enum eCastleFlag { wShortCastle = 1, wLongCastle = 2, bShortCastle = 4, bLongCastle = 8 };

// constant just outside piece range to denote no piece
static const int noPiece = 12;
