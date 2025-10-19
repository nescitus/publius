// Publius - Didactic public domain bitboard chess engine 
// by Pawel Koziol

// This file contains functions to detect captures
// that are expected to lose material. Only a single
// square is evaluated, so these functions are blind
// to overloaded pieces, pins and other tactical
// considerations.

#include "types.h"
#include "limits.h"
#include "position.h"
#include "move.h"
#include "bitboard.h"
#include "bitgen.h"
#include "badcapture.h"

// IsBadCapture() is used in two places:
// - in Quiesce() to prune captures that appear 
//   to lose material
// - in movepicker.cpp to sort such captures 
//   below the quiet moves

bool IsBadCapture(Position* pos, Move move) {

    const Square fromSquare = GetFromSquare(move);
    const Square toSquare = GetToSquare(move);
    const PieceType hunterType = pos->PieceTypeOnSq(fromSquare);
    const PieceType preyType = pos->PieceTypeOnSq(toSquare);

    // good or equal capture, based on simple piece values
    if (pieceValue[preyType] >= pieceValue[hunterType])
        return false;

    // en passant is an equal capture
    if (GetTypeOfMove(move) == tEnPassant)
        return false;

    // we must resort to static exchange evaluator
    return (Swap(pos, fromSquare, toSquare) < 0);
}

// Static Exchange Evaluator tries to calculate 
// the valueof an exchange on a single square 
// without playing out the moves. It is blind 
// to tactics, such as checks, pins, promotions 
// and overloaded defenders.

int Swap(const Position* pos, const Square fromSquare, const Square toSquare) {

    Color color;
    int ply, score[32];
    PieceType hunterType;
    Bitboard attackers, occupancy, newHunterMap;

    attackers = pos->AttacksTo(toSquare);
    occupancy = pos->Occupied();

    // put the value of the piece we are about 
    // to capture on the stack
    score[0] = pieceValue[pos->PieceTypeOnSq(toSquare)];

    // identify the capturing piece
    hunterType = pos->PieceTypeOnSq(fromSquare);

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

    // as long as there are the attackers of the right 
    // color...
    while (attackers & pos->Pieces(color)) {

        // special treatment of captures by king
        if (hunterType == King) {
            score[ply++] = Infinity;
            break;
        }
        
        // update the score
        score[ply] = -score[ply - 1] + pieceValue[hunterType];
        
        // find the lowest attacker type
        for (hunterType = Pawn; hunterType <= King; ++hunterType)
            if ((newHunterMap = pos->Map(color, hunterType) & attackers))
                break;

        // remove the new "hunter" from the occupancy map,
        // one piece at a time (there can be more than one 
        // piece of the same type attacking the toSquare). 
        // This  acts as a replacement of making  a  move.
        occupancy ^= (newHunterMap & -newHunterMap);
        
        // check if there are new captures available,
        // looking for pieces aligned with the "hunter"
        if (hunterType == Pawn || hunterType == Bishop || hunterType == Queen)
            attackers |= (GenerateMoves.Bish(occupancy, toSquare) & pos->AllDiagMovers());
        
        if (hunterType == Rook || hunterType == Queen)
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