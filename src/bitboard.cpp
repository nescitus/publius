// Publius - Didactic public domain bitboard chess engine by Pawel Koziol

#include "types.h"
#include "square.h" // for RelativeSq
#include "bitboard.h"

// convert square(s) to a corresponding bitboard

Bitboard Paint(const Square s) {
    return (Bitboard)1 << s;
}

Bitboard Paint(const Square s1, const Square s2) {
    return Paint(s1) | Paint(s2);
}

Bitboard Paint(const Square s1, const Square s2, const Square s3) {
    return Paint(s1,s2) | Paint(s3);
}

// color-dependent forward fill
Bitboard FrontSpan(const Bitboard b, const Color color) {

    return (color == White) ? FillNorth(NorthOf(b)) 
                            : FillSouth(SouthOf(b));
}

// Basic fill function (north and south only,
// as these are the most useful)

Bitboard FillNorth(Bitboard b) {
    b |= b << 8;
    b |= b << 16;
    b |= b << 32;
    return b;
}

Bitboard FillSouth(Bitboard b) {
    b |= b >> 8;
    b |= b >> 16;
    b |= b >> 32;
    return b;
}

// Occluded fill algorithms are taken from
// https://www.chessprogramming.org/Kogge-Stone_Algorithm#Occluded_Fill
// they do multiple shifts, shifting both the bitboard
// the bitboard and the "shadow" cast by the blocking 
// pieces. You would be able to generate rook moves
// by combining orthogonal fill functions and bishop
// moves by combining diagonal fill functions, but
// bitgen.cpp implements faster solution - namely 
// kindergarten bitboards.

Bitboard FillOcclSouth(Bitboard b, Bitboard o) {

    b |= o & (b >> 8);
    o &= (o >> 8);
    b |= o & (b >> 16);
    o &= (o >> 16);
    b |= o & (b >> 32);
    return b;
}

Bitboard FillOcclNorth(Bitboard b, Bitboard o) {

    b |= o & (b << 8);
    o &= (o << 8);
    b |= o & (b << 16);
    o &= (o << 16);
    b |= o & (b << 32);
    return b;
}

Bitboard FillOcclEast(Bitboard b, Bitboard o) {

    o &= excludeA;
    b |= o & (b << 1);
    o &= (o << 1);
    b |= o & (b << 2);
    o &= (o << 2);
    b |= o & (b << 4);
    return b;
}

Bitboard FillOcclNE(Bitboard b, Bitboard o) {

    o &= excludeA;
    b |= o & (b << 9);
    o &= (o << 9);
    b |= o & (b << 18);
    o &= (o << 18);
    b |= o & (b << 36);
    return b;
}

Bitboard FillOcclSE(Bitboard b, Bitboard o) {

    o &= excludeA;
    b |= o & (b >> 7);
    o &= (o >> 7);
    b |= o & (b >> 14);
    o &= (o >> 14);
    b |= o & (b >> 28);
    return b;
}

Bitboard FillOcclWest(Bitboard b, Bitboard o) {

    o &= excludeH;
    b |= o & (b >> 1);
    o &= (o >> 1);
    b |= o & (b >> 2);
    o &= (o >> 2);
    b |= o & (b >> 4);
    return b;
}

Bitboard FillOcclSW(Bitboard b, Bitboard o) {

    o &= excludeH;
    b |= o & (b >> 9);
    o &= (o >> 9);
    b |= o & (b >> 18);
    o &= (o >> 18);
    b |= o & (b >> 36);
    return b;
}

Bitboard FillOcclNW(Bitboard b, Bitboard o) {

    o &= excludeH;
    b |= o & (b << 7);
    o &= (o << 7);
    b |= o & (b << 14);
    o &= (o << 14);
    b |= o & (b << 28);
    return b;
}

// get bitboard of a relative square
Bitboard RelSqBb(const Color cl, const Square sq) {
    return (Paint(RelativeSq(cl, sq)));
}