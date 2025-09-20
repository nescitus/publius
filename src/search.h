// Publius - Didactic public domain bitboard chess engine by Pawel Koziol

#pragma once

extern int lastCaptureTarget[64];

void Iterate(Position* pos);
int Widen(Position* pos, int depth, int lastScore);
int Search(Position* pos, int ply, int alpha, int beta, int depth, bool wasNullMove, bool isExcluded);
int Quiesce(Position* pos, int ply, int qdepth, int alpha, int beta);
bool SetImproving(int eval, int ply);
void PrintRootInfo();
void TryInterrupting(void);