// Publius - Didactic public domain bitboard chess engine by Pawel Koziol

#include "types.h"
#include "bitboard.h"

 #define FAST_POPCNT
// uncomment this when compiling for modern Windows systems
// to gain a little speedup.

#ifdef FAST_POPCNT
#include <nmmintrin.h>
#include <intrin.h>
#endif

// PopCnt() - shorthand for population count -
// returns the number of ones in the bitboard.
// It is used for example in evaluating mobility.

#ifdef FAST_POPCNT

int PopCnt(Bitboard b) {
    return (int)_mm_popcnt_u64(b);
}

#else

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

#endif

// FirstOne() finds the first bit 
// in the bitboard. It is used for breaking
// the bitboard into squares ("serializing"),
// which is the basic building block
// of the move generator. As it requires
// removing the bits from the serialized
// bitboard, we use PopFirstBit() function
// for that.

#ifdef FAST_POPCNT

Square FirstOne(Bitboard b) {
    unsigned long index;
    _BitScanForward64(&index, b);
    return (Square)index;
}

#else

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

Square FirstOne(Bitboard b) {
    return static_cast<Square>(bitTable[(((b) & (~(b)+1)) * (Bitboard)0x0218A392CD3D5DBF) >> 58]);
}

#endif

// return the first bit and clear it from the bitboard
Square PopFirstBit(Bitboard* b) {

    Bitboard bb = *b;
    *b &= (*b - 1);
    return FirstOne(bb);
}