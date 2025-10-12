// Publius - Didactic public domain bitboard chess engine 
// by Pawel Koziol

#pragma once

#include "move.h"

constexpr Move dummyMove = CreateMove(A1, B8, 0); // clearly illegal

struct Stack {
	int previousCaptureTo; // target square or previous capture if applicable, else -1
	int previousEval; // eval for each ply, to see if we are improving or not
};

struct SearchContext {
	Stack stack[SearchTreeSize];
	Move excludedMove;
};

void ClearSearchContext(SearchContext& sc);
void Iterate(Position* pos, SearchContext* sc);
int Widen(Position* pos, SearchContext* sc, int depth, int lastScore);
int Search(Position* pos, SearchContext* sc, int ply, int alpha, int beta, int depth, bool wasNullMove, bool isExcluded);
int Quiesce(Position* pos, int ply, int qdepth, int alpha, int beta);
bool SetImproving(const Stack &ppst, int eval, int ply);
void PrintRootInfo();
void TryInterrupting(void);