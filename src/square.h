/*
Define Color enum and what can be done with it.

Type safety is explained in types.h
*/

#pragma once
#include <iostream>

Square MakeSquare(const int rank, const int file);
int RankOf(const Square square);
int FileOf(const Square square);
Square InvertSquare(const Square square);
Square RelativeSq(const Color color, const Square square);
std::string SquareName(Square sq);
int AbsoluteDelta(Square s1, Square s2);