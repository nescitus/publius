// Publius - Didactic public domain bitboard chess engine by Pawel Koziol

#pragma once
#include <iostream>

Square MakeSquare(const int rank, const int file);
int RankOf(const Square square);
int FileOf(const Square square);
bool IsSameRank(const Square s1, const Square s2);
bool IsSameFile(const Square s1, const Square s2);
bool IsSameUpwardsDiag(const Square s1, const Square s2);
bool IsSameDownwardsDiag(const Square s1, const Square s2);
Square MirrorRank(const Square square);
Square MirrorFile(const Square square);
Square RelativeSq(const Color color, const Square square);
std::string SquareName(Square sq);
int AbsoluteDelta(Square s1, Square s2);