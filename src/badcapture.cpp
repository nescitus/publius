#include "color.h"
#include "piece.h"
#include "square.h"
#include "limits.h"
#include "publius.h"
#include "move.h"
#include "bitboard.h"
#include "bitgen.h"

const int tp_value[6] = { 100, 300, 300, 500, 900, 0 };

// IsBadCapture() is used in two places:
// - in Quiesce() to prune captures that appear to lose material
// - in ScoreMoves() to sort such captures below the quiet moves

bool IsBadCapture(Position* pos, int move) {

    Square fromSquare = GetFromSquare(move);
    Square toSquare = GetToSquare(move);
    int hunter = pos->PieceTypeOnSq(fromSquare);
    int prey = pos->PieceTypeOnSq(toSquare);

    // good or equal capture, based on simple piece values
    if (tp_value[prey] >= tp_value[hunter])
        return false;

    // en passant is an equal capture
    if (GetTypeOfMove(move) == tEnPassant)
        return false;

    // static exchange evaluator
    if (Swap(pos, fromSquare, toSquare) < 0)
        return true;
    
    return false;
}

// Static Exchange Evaluator tries to calculate the value
// of an exchange on a single square without playing out
// the moves. It is blind to tactics, such as checks, pins,
// promotions and overloaded defenders.

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

    // we are now evaluating from the defender's POV
    color = ~pos->GetSideToMove();

    // we have completed the capture that is evaluated
    ply = 1;

    // as long as there are the attackers of the right color...
    while (attackers & pos->Map(color)) {

        // special treatment of captures by king
        if (hunter == King) {
            score[ply++] = Infinity;
            break;
        }
        
        // update the score
        score[ply] = -score[ply - 1] + tp_value[hunter];
        
        // find the lowest attacker type
        for (hunter = Pawn; hunter <= King; hunter++)
            if ((newHunterMap = pos->Map(color, hunter) & attackers))
                break;

        // remove the new "hunter" from the occupancy map,
        // one piece at a time (there can be more than one
        // piece of the same type attacking the toSquare)
        occupancy ^= (newHunterMap & -newHunterMap);
        
        // check if there are new captures available,
        // looking for pieces aligned with the "hunter"
        if (hunter == Pawn || hunter == Bishop || hunter == Queen)
            attackers |= (GenerateMoves.Bish(occupancy, toSquare) & pos->AllDiagMovers());
        if (hunter == Rook || hunter == Queen)
            attackers |= (GenerateMoves.Rook(occupancy, toSquare) & pos->AllStraightMovers());
        
        // proceed to the next ply
        attackers &= occupancy;
        color = ~color;
        ply++;
    }

    // minimax the score stack
    while (--ply)
        score[ply - 1] = -std::max(-score[ply - 1], score[ply]);
    return score[0];
}