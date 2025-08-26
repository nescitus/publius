// Publius - Didactic public domain bitboard chess engine 
// by Pawel Koziol

// Defining basic types and operators applied to them

#pragma once

typedef unsigned long long Bitboard;
typedef int Move;

// ---------- Color ----------
// https://github.com/nescitus/publius/wiki/Color

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

// ----- ColoredPiece --------

enum ColoredPiece { WhitePawn, BlackPawn, WhiteKnight, BlackKnight, 
                    WhiteBishop, BlackBishop, WhiteRook, BlackRook, 
                    WhiteQueen, BlackQueen, WhiteKing, BlackKing, 
                    noPiece };

// increment ColoredPiece in a "for" loop
constexpr inline ColoredPiece operator++(ColoredPiece& d) {
    return d = ColoredPiece(int(d) + 1);
}

// ------- PieceType ----------

enum PieceType { Pawn, Knight, Bishop, Rook, Queen, King, noPieceType };

// increment PieceType in a "for" loop
constexpr inline PieceType operator++(PieceType& d) {
    return d = PieceType(int(d) + 1);
}

// --------- Square ------------

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

// needed for mirroring
constexpr inline Square operator^(Square d1, int d2) {
    return Square(int(d1) ^ d2);
}

// --------- Others ------------

// ranks and files
enum File { fileA, fileB, fileC, fileD, fileE, fileF, fileG, fileH };
enum Rank { rank1, rank2, rank3, rank4, rank5, rank6, rank7, rank8 };
enum MoveType { tNormal, tCastle, tEnPassant, tPawnjump, tPromN, tPromB, tPromR, tPromQ };
enum CastleFlag { wShortCastle = 1, wLongCastle = 2, bShortCastle = 4, bLongCastle = 8 };
