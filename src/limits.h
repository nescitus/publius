#pragma once

constexpr int Infinity = 32767;
constexpr int MateScore = 32000;
constexpr int EvalLimit = 29999;
constexpr int PlyLimit = 64;
constexpr int MovesLimit = 256;
constexpr int ScoreDraw = 0;

// for move sorting
constexpr int IntLimit = 2147483646;
constexpr int HighValue = IntLimit / 2;
constexpr int GoodCaptureValue = HighValue + 100;
constexpr int BadCaptureValue = 100;
constexpr int Killer1Value = HighValue;
constexpr int Killer2Value = HighValue - 1;
constexpr int QueenPromValue = HighValue + 110; // TODO: try lower than killers
constexpr int KnightPromValue = HighValue + 109;
constexpr int RookPromValue = HighValue + 108;
constexpr int BishopPromValue = HighValue + 107;
constexpr int HistLimit = (1 << 15);