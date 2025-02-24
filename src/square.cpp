#include "publius.h"

// given rank and file, define square

Square MakeSquare(int rank, int file) {
    return (Square) ((file << 3) | rank);
}

// get rank that a square belongs to

int RankOf(Square square) {
    return square >> 3;
}

// get file that a square belongs to

int FileOf(Square square) {
    return  square & 7;
}

// get vertical reflection of a square
// (i.e. reflection of C2 is C7)

Square InvertSquare( Square square) {
    return square ^ 56;
}

// get square relative to the side to move
// - RelativeSquare(White, C2) is C2,
// - RelativeSquare(Black, C2) is C7

Square RelativeSq(Color color, Square square) {
    return square ^ (color * 56);
}