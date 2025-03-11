// Publius 1.0. Didactic bitboard chess engine by Pawel Koziol

#include "color.h"
#include "square.h"
#include "limits.h"
#include "publius.h"
#include "evaldata.h"
#include "eval.h"
#include "piece.h"

void Parameters::Init() {
    InitPst();
}

void Parameters::InitPst(void) {

	for (Color color = White; color < colorNone; ++color) {
		for (Square square = A1; square < sqNone; ++square) {

            mgPst[color][Pawn][RelativeSq(color, square)] = mgPawnPst[square];
            egPst[color][Pawn][RelativeSq(color, square)] = egPawnPst[square];
            shield[color][RelativeSq(color, square)] = kingShield[square];
            storm[color][RelativeSq(color, square)] = kingStorm[square];

            mgPst[color][Knight][RelativeSq(color, square)] = mgKnightPst[square];
            egPst[color][Knight][RelativeSq(color, square)] = egKnightPst[square];

            mgPst[color][Bishop][RelativeSq(color, square)] = mgBishopPst[square];
            egPst[color][Bishop][RelativeSq(color, square)] = egBishopPst[square];

            mgPst[color][Rook][RelativeSq(color, square)] = mgRookPst[square];
            egPst[color][Rook][RelativeSq(color, square)] = egRookPst[square];

            mgPst[color][Queen][RelativeSq(color, square)] = mgQueenPst[square];
            egPst[color][Queen][RelativeSq(color, square)] = egQueenPst[square];

            mgPst[color][King][RelativeSq(color, square)] = mgKingPst[square];
            egPst[color][King][RelativeSq(color, square)] = egKingPst[square];
		}
	}
}