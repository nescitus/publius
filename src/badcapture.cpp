#include "color.h"
#include "piece.h"
#include "square.h"
#include "publius.h"
#include "move.h"
#include "bitboard.h"
#include "bitgen.h"

const int tp_value[6] = { 100, 300, 300, 500, 900, 0 };

bool IsBadCapture(Position* pos, int move) {

    Square from = GetFromSquare(move);
    Square to = GetToSquare(move);
    int hunter = pos->PieceTypeOnSq(from);
    int prey = pos->PieceTypeOnSq(to);

    Bitboard bbAttackers, bbDefenders, bbOcc, bbType;
    Color oppo = ~pos->GetSideToMove();

    // good or equal capture
    if (tp_value[prey] >= tp_value[hunter])
        return false;

  // non-pawn defended by a pawn
   if (prey == Pawn && hunter != Pawn &&
       pos->IsPawnDefending(oppo, to))
    return true;

    // TODO: static exchange evaluator
    //if (pos->Swap(from, to) < 0)
    //    return true;

    return false;
}