// Publius - Didactic public domain bitboard chess engine by Pawel Koziol

#include "types.h"
#include "square.h"
#include "publius.h"
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

// Functions to shift a bitboard in the given direction.
// They are named after cardinal directions and we assume
// that white moves north, that is up the board,
// like on a chess diagram.

Bitboard NorthOf(const Bitboard b) {
    return(b << 8);
}

Bitboard SouthOf(const Bitboard b) {
    return (b >> 8);
}

Bitboard WestOf(const Bitboard b) {
    return ((b & excludeA) >> 1);
}

Bitboard EastOf(const Bitboard b) {
    return ((b & excludeH) << 1);
}

Bitboard NWOf(const Bitboard b) {
    return ((b & excludeA) << 7);
}

Bitboard NEOf(const Bitboard b) {
    return ((b & excludeH) << 9);
}

Bitboard SWOf(const Bitboard b) {
    return ((b & excludeA) >> 9);
}

Bitboard SEOf(const Bitboard b) {
    return ((b & excludeH) >> 7);
}

// get pawn bitboard attacks

Bitboard GetWPAttacks(const Bitboard b) {
    return (NEOf(b) | NWOf(b));
}

Bitboard GetBPAttacks(const Bitboard b) {
    return (SEOf(b) | SWOf(b));
}

// shift the bitboard to both sides
Bitboard SidesOf(const Bitboard b) {
    return (WestOf(b) | EastOf(b));
}

// color-dependent forward shift
Bitboard ForwardOf(const Bitboard b, const Color color) {
    return (color == White) ? NorthOf(b) : SouthOf(b);
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