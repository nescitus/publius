// Publius - Didactic public domain bitboard chess engine by Pawel Koziol

#pragma once
#include <iostream>

// get rank that a square belongs to
constexpr inline int RankOf(Square square) { return square >> 3; }

// get file that a square belongs to
constexpr inline int FileOf(Square square) { return  square & 7; }

// get horizontal reflection of a square (A <-> H)
constexpr inline Square MirrorFile(const Square square) {
    return Square(square ^ 7);
}

// get vertical reflection of a square (1 <-> 8)
// - MirrorRank(C2) is C7,
// - MirrorRank(C7) is C2
constexpr inline Square MirrorRank(const Square square) {
    return square ^ 56;
}

// get square relative to the side to move
// - RelativeSquare(White, C2) is C2,
// - RelativeSquare(Black, C2) is C7
constexpr inline Square RelativeSq(Color c, Square s) {
    return Square(int(s) ^ (int(c) * 56)); // relies on White=0, Black=1
}

// Same-line predicates
constexpr inline bool IsSameRank(Square a, Square b) { 
    return RankOf(a) == RankOf(b); 
}

constexpr inline bool IsSameFile(Square a, Square b) { 
    return FileOf(a) == FileOf(b); 
}

// Diagonals: up-left/down-right and up-right/down-left
constexpr inline bool IsSameUpwardsDiag(Square a, Square b) noexcept { // NW-SE
    return (FileOf(a) - RankOf(a)) == (FileOf(b) - RankOf(b));
}
constexpr inline bool IsSameDownwardsDiag(Square a, Square b) noexcept { // NE-SW
    return (FileOf(a) + RankOf(a)) == (FileOf(b) + RankOf(b));
}

constexpr inline bool IsSameRankOrFile(Square a, Square b) noexcept {
    return IsSameRank(a, b) || IsSameFile(a, b);
}
constexpr inline bool IsSameDiagonal(Square a, Square b) noexcept {
    return IsSameUpwardsDiag(a, b) || IsSameDownwardsDiag(a, b);
}

Square MakeSquare(const int rank, const int file);
std::string SquareName(Square sq);
int AbsoluteDelta(Square s1, Square s2);