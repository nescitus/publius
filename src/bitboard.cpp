// Publius 1.0. Didactic bitboard chess engine by Pawel Koziol

#include "publius.h"
#include "bitboard.h"

const int bitTable[64] = {
    0,  1,  2,  7,  3, 13,  8, 19,
    4, 25, 14, 28,  9, 34, 20, 40,
    5, 17, 26, 38, 15, 46, 29, 48,
   10, 31, 35, 54, 21, 50, 41, 57,
   63,  6, 12, 18, 24, 27, 33, 39,
   16, 37, 45, 47, 30, 53, 49, 56,
   62, 11, 23, 32, 36, 44, 52, 55,
   61, 22, 43, 51, 60, 42, 59, 58
};

Bitboard Paint(Square s) {
    return (Bitboard)1 << s;
}

Bitboard Paint(Square s1, Square s2) {
    return Paint(s1) | Paint(s2);
}

Bitboard Paint(Square s1, Square s2, Square s3) {
    return Paint(s1,s2) | Paint(s3);
}

Square FirstOne(Bitboard b) {
    return (Square)(bitTable[(((b) & (~(b)+1)) * (Bitboard)0x0218A392CD3D5DBF) >> 58]);
}

int PopCnt(Bitboard b) {

    Bitboard k1 = (Bitboard)0x5555555555555555;
    Bitboard k2 = (Bitboard)0x3333333333333333;
    Bitboard k3 = (Bitboard)0x0F0F0F0F0F0F0F0F;
    Bitboard k4 = (Bitboard)0x0101010101010101;

    b -= (b >> 1) & k1;
    b = (b & k2) + ((b >> 2) & k2);
    b = (b + (b >> 4)) & k3;
    return (b * k4) >> 56;
}

Square PopFirstBit(Bitboard * b) {

    Bitboard bb = *b;
    *b &= (*b - 1);
    return FirstOne(bb);
}

Bitboard NorthOf(Bitboard b) {
    return(b << 8);
}

Bitboard SouthOf(Bitboard b) {
    return (b >> 8);
}

Bitboard WestOf(Bitboard b) {
    return ((b & excludeA) >> 1);
}

Bitboard EastOf(Bitboard b) {
    return ((b & excludeH) << 1);
}

Bitboard NWOf(Bitboard b) {
    return ((b & excludeA) << 7);
}

Bitboard NEOf(Bitboard b) {
    return ((b & excludeH) << 9);
}

Bitboard SWOf(Bitboard b) {
    return ((b & excludeA) >> 9);
}

Bitboard SEOf(Bitboard b) {
    return ((b & excludeH) >> 7);
}

Bitboard GetWPAttacks(Bitboard b) {
    return (NEOf(b) | NWOf(b));
}

Bitboard GetBPAttacks(Bitboard b) {
    return (SEOf(b) | SWOf(b));
}

Bitboard SidesOf(Bitboard b) {
    return (WestOf(b) | EastOf(b));
}

Bitboard ForwardOf(Bitboard b, Color c) {

    if (c == White) return NorthOf(b);
    else return SouthOf(b);
}

Bitboard FrontSpan(Bitboard b, Color c)
{
    if (c == White) return FillNorth(NorthOf(b));
    else            return FillSouth(SouthOf(b));
}

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