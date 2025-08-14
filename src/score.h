// Publius - Didactic public domain bitboard chess engine by Pawel Koziol

#pragma once

// We pack (mg, eg) score into a single integer.
// This gais some speed (by not adding the values
// twice), but more importantly simplifies the code.

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