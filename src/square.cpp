#include "color.h"
#include "square.h"
#include <iostream>

// step through squares, usually in a "for" loop
Square operator++(Square& d) { 
    return d = Square(int(d) + 1); 
}

// add vector to a square
Square operator+(Square d1, int d2) { 
    return Square(int(d1) + int(d2)); 
}

// substract vector from a square
Square operator-(Square d1, int d2) { 
    return Square(int(d1) - int(d2)); 
}

// needed for en passant
Square operator^(Square d1, int d2) { 
    return Square(int(d1) ^ d2); 
}

// given rank and file, define square
Square MakeSquare(const int rank, int file) {
    return static_cast<Square>((file << 3) | rank);
}

// get rank that a square belongs to
int RankOf(const Square square) {
    return square >> 3;
}

// get file that a square belongs to
int FileOf(const Square square) {
    return  square & 7;
}

// get vertical reflection of a square
// (i.e. reflection of C2 is C7)
Square InvertSquare(const Square square) {
    return square ^ 56;
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