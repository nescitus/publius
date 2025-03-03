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
   
   bbAttackers = pos->AttacksTo(to);

    // static exchange evaluator
    if (Swap(pos, from, to) < 0)
        return true;
    
    return false;
}

int Swap(Position *pos, Square fromSquare, Square toSquare) {

    Color color;
    int ply, hunter, score[32];
    Bitboard attackers, occupancy, newHunterMap;

    attackers = pos->AttacksTo(toSquare);
    occupancy = pos->Occupied();

    // put the value of the piece we are about to capture
    // on the stack
    score[0] = tp_value[pos->PieceTypeOnSq(toSquare)];

    // identify the capturing piece
    hunter = pos->PieceTypeOnSq(fromSquare);

    // update occupancy map, removing the first capturer
    occupancy ^= Paint(fromSquare);

    // add the attacks by the sliding pieces 
    // through the first capturer
    attackers |= (GenerateMoves.Bish(occupancy, toSquare) & pos->AllDiagMovers())
              | (GenerateMoves.Rook(occupancy, toSquare) & pos->AllStraightMovers());
    attackers &= occupancy;

    // switch side
    color = ~pos->GetSideToMove();

    // we have completed the capture that is evaluated
    ply = 1;

    while (attackers & pos->Map(color)) {

        // special treatment of captures by king
        if (hunter == King) {
            score[ply++] = Infinity;
            break;
        }
        
        score[ply] = -score[ply - 1] + tp_value[hunter];
        
        for (hunter = Pawn; hunter <= King; hunter++)
            if ((newHunterMap = pos->Map(color, hunter) & attackers))
                break;

        occupancy ^= (newHunterMap & -newHunterMap);
        
        if (hunter == Pawn || hunter == Bishop || hunter == Queen)
            attackers |= (GenerateMoves.Bish(occupancy, toSquare) & pos->AllDiagMovers());
        if (hunter == Rook || hunter == Queen)
            attackers |= (GenerateMoves.Rook(occupancy, toSquare) & pos->AllStraightMovers());
        
        attackers &= occupancy;
        color = ~color;
        ply++;
    }

    // minimax the score stack
    while (--ply)
        score[ply - 1] = -std::max(-score[ply - 1], score[ply]);
    return score[0];
}