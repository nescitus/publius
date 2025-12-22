// Publius - Didactic public domain bitboard chess engine 
// by Pawel Koziol

// Functions for packing and unpacking (mg, eg) 
// score into a single integer.  Such composite
// scores can be added and substracted (but not
// multiplied or divided). This gais some speed 
// (by not adding the values twice), decreases
// the size of eval and pawn transposition tables,
// but more importantly simplifies the code.

#pragma once
#include <cstdint>

constexpr int MakeScore(int mg, int eg) {
    return ((unsigned int)(eg) << 16) + mg;
}

constexpr int ScoreMG(int s) {
    return (int16_t)((uint16_t)((unsigned)(s)));
}

constexpr int ScoreEG(int s) {
    return (int16_t)((uint16_t)((unsigned)(s + 0x8000) >> 16));
}

#define S(mg, eg) MakeScore(mg, eg)
