// Publius - Didactic public domain bitboard chess engine by Pawel Koziol

#pragma once

constexpr int Infinity = 32767;
constexpr int MateScore = 32000;
constexpr int EvalLimit = 29999;
constexpr int PlyLimit = 64;
constexpr int ScoreDraw = 0;
constexpr int HistLimit = (1 << 15);