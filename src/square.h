/*
Define Color enum and what can be done with it.

Type safety is explained in color.h
*/

#pragma once

enum eFile { fileA, fileB, fileC, fileD, fileE, fileF, fileG, fileH };
enum eRank { rank1, rank2, rank3, rank4, rank5, rank6, rank7, rank8 };
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
Square operator++(Square& d);

// add vector to a square
Square operator+(Square d1, int d2);

// substract vector from a square
Square operator-(Square d1, int d2);

// needed for en passant
Square operator^(Square d1, int d2);

Square MakeSquare(const int rank, const int file);
int RankOf(const Square square);
int FileOf(const Square square);
Square InvertSquare(const Square square);
Square RelativeSq(const Color color, const Square square);