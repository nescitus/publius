#include "color.h"
#include "piece.h"
#include "square.h"
#include "limits.h"
#include "publius.h"
#include "move.h"
#include "bitboard.h"
#include "bitgen.h"

const int tp_value[6] = { 100, 300, 300, 500, 900, 0 };

bool IsBadCapture(Position* pos, int move) {
    
    Bitboard bbAttackers, bbDefenders, bbOcc, bbType;
    int type;

    Color oppo = ~pos->GetSideToMove();
    Square from = GetFromSquare(move);
    Square to = GetToSquare(move);
    int hunter = pos->PieceTypeOnSq(from);
    int prey = pos->PieceTypeOnSq(to);

    // good or equal capture
    if (tp_value[prey] >= tp_value[hunter])
        return false;

  // non-pawn defended by a pawn
   if (prey == Pawn && hunter != Pawn &&
       pos->IsPawnDefending(oppo, to))
    return true;

   
   bbAttackers = pos->AttacksTo(to);

   // didn't find a defender
   if (!(bbAttackers & pos->Map(oppo)))
       return false;

   // get the lowest defender type
   for (type = Pawn; type <= King; type++)
       if ((bbType = pos->Map(oppo, type) & bbAttackers))
           break;

   // defender can recapture
   // and attacker's optimistic scenario
   // (capturing the defending piece) is insufficient
   if (tp_value[prey] + tp_value[type] < tp_value[hunter] )
       return true;

    // TODO: static exchange evaluator
    //if (Swap(from, to) < 0)
    //    return true;
    
    return false;
}