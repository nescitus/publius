// Publius 1.0. Didactic bitboard chess engine by Pawel Koziol

#include "color.h"
#include "square.h"
#include "publius.h"
#include "eval.h"
#include "piece.h"

void InitPst(void) {

	for (Color color = White; color < colorNone; ++color) {
		for (Square square = A1; square < sqNone; ++square) {

            mgTable[color][Pawn][RelativeSq(color, square)] = mgPawnPst[square];
            egTable[color][Pawn][RelativeSq(color, square)] = egPawnPst[square];

            mgTable[color][Knight][RelativeSq(color, square)] = mgKnightPst[square];
            egTable[color][Knight][RelativeSq(color, square)] = egKnightPst[square];

            mgTable[color][Bishop][RelativeSq(color, square)] = mgBishopPst[square];
            egTable[color][Bishop][RelativeSq(color, square)] = egBishopPst[square];

            mgTable[color][Rook][RelativeSq(color, square)] = mgRookPst[square];
            egTable[color][Rook][RelativeSq(color, square)] = egRookPst[square];

            mgTable[color][Queen][RelativeSq(color, square)] = mgQueenPst[square];
            egTable[color][Queen][RelativeSq(color, square)] = egQueenPst[square];

            mgTable[color][King][RelativeSq(color, square)] = mgKingPst[square];
            egTable[color][King][RelativeSq(color, square)] = egKingPst[square];
		}
	}
}