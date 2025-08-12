// Publius - Didactic public domain bitboard chess engine by Pawel Koziol

#pragma once
#include <iostream>

// get rank that a square belongs to
constexpr inline int RankOf(Square square) { return square >> 3; }

// get file that a square belongs to
constexpr inline int FileOf(Square square) { return  square & 7; }

Square MakeSquare(const int rank, const int file);
Square MirrorRank(const Square square);
Square MirrorFile(const Square square);
bool IsSameRank(const Square s1, const Square s2);
bool IsSameFile(const Square s1, const Square s2);
bool IsSameUpwardsDiag(const Square s1, const Square s2);
bool IsSameDownwardsDiag(const Square s1, const Square s2);
Square RelativeSq(const Color color, const Square square);
std::string SquareName(Square sq);
int AbsoluteDelta(Square s1, Square s2);