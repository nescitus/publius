// Publius - Didactic public domain bitboard chess engine by Pawel Koziol

#include "types.h"
#include "square.h"
#include <iostream>

// given rank and file, define square
Square MakeSquare(const int rank, int file) {
    return static_cast<Square>((file << 3) | rank);
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