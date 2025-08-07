// Publius - Didactic public domain bitboard chess engine by Pawel Koziol

#include "types.h"
#include "square.h"
#include "publius.h"
#include "evaldata.h"
#include "eval.h"

void Parameters::Init(void) {

    for (Color color = White; color < colorNone; ++color) {
        for (Square square = A1; square < sqNone; ++square) {

            pawnSupport[color][RelativeSq(color, square)] = S(p_support[square],0);
            pst[color][Pawn][RelativeSq(color, square)] = pieceValue[Pawn] + pawnPst[square];
            pst[color][Knight][RelativeSq(color, square)] = pieceValue[Knight] + knightPst[square];
            pst[color][Bishop][RelativeSq(color, square)] = pieceValue[Bishop] + bishopPst[square];
            pst[color][Rook][RelativeSq(color, square)] = pieceValue[Rook] + rookPst[square];
            pst[color][Queen][RelativeSq(color, square)] = pieceValue[Queen] + queenPst[square];
            pst[color][King][RelativeSq(color, square)] = kingPst[square];
        }
    }
}