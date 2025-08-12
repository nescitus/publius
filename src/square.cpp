// Publius - Didactic public domain bitboard chess engine by Pawel Koziol

#include "types.h"
#include "square.h"
#include <iostream>

static const int upwardsDiagonal[64] = {
       0,   1,   2,   3,   4,   5,   6,   7,
       1,   2,   3,   4,   5,   6,   7,   8,
       2,   3,   4,   5,   6,   7,   8,   9,
       3,   4,   5,   6,   7,   8,   9,  10,
       4,   5,   6,   7,   8,   9,  10,  11,
       5,   6,   7,   8,   9,  10,  11,  12,
       6,   7,   8,   9,  10,  11,  12,  13,
       7,   8,   9,  10,  11,  12,  13,  14
};

static const int downwardsDiagonal[64] = {
       7,   8,   9,  10,  11,  12,  13,  14,
       6,   7,   8,   9,  10,  11,  12,  13,
       5,   6,   7,   8,   9,  10,  11,  12,
       4,   5,   6,   7,   8,   9,  10,  11,
       3,   4,   5,   6,   7,   8,   9,  10,
       2,   3,   4,   5,   6,   7,   8,   9,
       1,   2,   3,   4,   5,   6,   7,   8,
       0,   1,   2,   3,   4,   5,   6,   7
};

// given rank and file, define square
Square MakeSquare(const int file, const int rank) {
    return static_cast<Square>((file << 3) | rank);
}

// rank, file and diagonal comparison routines

bool IsSameRank(const Square s1, const Square s2) {
    return RankOf(s1) == RankOf(s2);
}

bool IsSameFile(const Square s1, const Square s2) {
    return FileOf(s1) == FileOf(s2);
}

bool IsSameUpwardsDiag(const Square s1, const Square s2) {
    return upwardsDiagonal[s1] == upwardsDiagonal[s2];
}

bool IsSameDownwardsDiag(const Square s1, const Square s2) {
    return downwardsDiagonal[s1] == downwardsDiagonal[s2];
}

bool IsSameRankOrFile(const Square s1, const Square s2) {
    return IsSameRank(s1, s2) || IsSameFile(s1, s2);
}

bool IsSameDiagonal(const Square s1, const Square s2) {
    return IsSameUpwardsDiag(s1, s2) || IsSameDownwardsDiag(s1, s2);
}

// get vertical reflection of a square
// - InvertSquare(C2) is C7, 
// - InvertSquare(C7) is C2
Square MirrorRank(const Square square) {  // 1 <-> 8
    return square ^ 56;
}

Square MirrorFile(const Square square) { // A <-> H
    return Square(square ^ 7); 
}

// get square relative to the side to move
// - RelativeSquare(White, C2) is C2,
// - RelativeSquare(Black, C2) is C7
Square RelativeSq(const Color color, const Square square) {
    return square ^ (color * 56);
}

std::string SquareName(Square sq) {
    
    std::string result;
    result += FileOf(sq) + 'a';
    result += RankOf(sq) + '1';
    return result;
}

// needed to get rid of a compiler warning
// in move.cpp (castle and pawn jump detection)
// Might be removed if we find a sounder logic.
int AbsoluteDelta(Square s1, Square s2) {
    return std::abs((int)s1 - (int(s2)));
}