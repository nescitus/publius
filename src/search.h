#pragma once

extern int oldEval[PlyLimit];

void Iterate(Position* pos);
int Widen(Position* pos, int depth, int lastScore);
int Search(Position* pos, int ply, int alpha, int beta, int depth, bool wasNull);
int Quiesce(Position* pos, int ply, int qdepth, int alpha, int beta);
int GetMoveType(Position* pos, int move, int ttMove);
void PrintRootInfo(int elapsed, int nps);
void Think(Position* pos);
int Timeout(void);