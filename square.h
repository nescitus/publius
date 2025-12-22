// Publius - Didactic public domain bitboard chess engine 
// by Pawel Koziol

#pragma once

// given rank and file, define square
constexpr inline Square MakeSquare(const int rank, int file) {
    return static_cast<Square>((file << 3) | rank);
}

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
// - RelativeSq(White, C2) is C2,
// - RelativeSq(Black, C2) is C7
constexpr inline Square RelativeSq(Color c, Square s) {
    return Square(int(s) ^ (int(c) * 56)); // relies on White=0, Black=1
}

// Comparing straight ranks
constexpr inline bool IsSameRank(Square a, Square b) { 
    return RankOf(a) == RankOf(b); 
}

constexpr inline bool IsSameFile(Square a, Square b) { 
    return FileOf(a) == FileOf(b); 
}

// Comparing diagonals: up-left/down-right and up-right/down-left
constexpr inline bool IsSameUpwardsDiag(Square a, Square b) noexcept { // NW-SE
    return (FileOf(a) - RankOf(a)) == (FileOf(b) - RankOf(b));
}
constexpr inline bool IsSameDownwardsDiag(Square a, Square b) noexcept { // NE-SW
    return (FileOf(a) + RankOf(a)) == (FileOf(b) + RankOf(b));
}

// Comparing orthogonal/diagonal
constexpr inline bool IsSameRankOrFile(Square a, Square b) noexcept {
    return IsSameRank(a, b) || IsSameFile(a, b);
}
constexpr inline bool IsSameDiagonal(Square a, Square b) noexcept {
    return IsSameUpwardsDiag(a, b) || IsSameDownwardsDiag(a, b);
}

// Absolute distance in the linear 0..63 space (use carefully)
// currently pawnjump and castle detection depends on it,
// but it might go if we improve this logic
constexpr inline int AbsoluteDelta(Square a, Square b) noexcept {
    return (int(a) > int(b)) ? (int(a) - int(b)) : (int(b) - int(a));
}