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

            mgPst[color][Pawn][RelativeSq(color, square)] = mgPawnPst[square] + mgPieceValue[Pawn];
            egPst[color][Pawn][RelativeSq(color, square)] = egPawnPst[square] + egPieceValue[Pawn];

            mgPst[color][Knight][RelativeSq(color, square)] = mgKnightPst[square] + mgPieceValue[Knight];
            egPst[color][Knight][RelativeSq(color, square)] = egKnightPst[square] + egPieceValue[Knight];

            mgPst[color][Bishop][RelativeSq(color, square)] = mgBishopPst[square] + mgPieceValue[Bishop];
            egPst[color][Bishop][RelativeSq(color, square)] = egBishopPst[square] + egPieceValue[Bishop];

            mgPst[color][Rook][RelativeSq(color, square)] = mgRookPst[square] + mgPieceValue[Rook];
            egPst[color][Rook][RelativeSq(color, square)] = egRookPst[square] + egPieceValue[Rook];

            mgPst[color][Queen][RelativeSq(color, square)] = mgQueenPst[square] + mgPieceValue[Queen];
            egPst[color][Queen][RelativeSq(color, square)] = egQueenPst[square] + egPieceValue[Queen];

            mgPst[color][King][RelativeSq(color, square)] = mgKingPst[square];
            egPst[color][King][RelativeSq(color, square)] = egKingPst[square];
		}
	}
}