// Publius - Didactic public domain bitboard chess engine 
// by Pawel Koziol

// This  file contains functions detecting whether a move  is 
// pseudo-legal. This means testing all the conditions except
// the in check status, which is dealt with during the search.

#include "types.h"
#include "piece.h"
#include "square.h" // for rank and file
#include "bitboard.h"
#include "position.h"
#include "move.h"
#include "legality.h"
#include "publius.h"
#include <iostream>

// Detect if a move is pseudo-legal
bool IsPseudoLegal(Position* pos, int move) {

    // NOTE: a move is not guaranteed to work in the  current
    // position,  this is why we are testing it in the  first
    // place. The less obvious implication is that we  cannot 
    // trust the move flag. For example, you can have a legal
    // d2d4 move by a queen and compare it with an input move 
    // flagged  as a pawn jump. I learned this the  hard  way 
    // during an unsuccessful refactor.

    if (move == 0) 
        return false;

    // Create move description (better than loose variables)
    const MoveDescription md(*pos, move);

    // From square is empty or enemy piece occupies it
    if (md.hunter == noPieceType || 
        ColorOfPiece(pos->GetPiece(md.fromSquare)) != md.side)
        return false;

    // To square is empty or an own piece occupies it
    if (md.prey != noPieceType && 
        ColorOfPiece(pos->GetPiece(md.toSquare)) == md.side)
        return false;       

    // Castling
    if (md.type == tCastle)
        return IsCastlingLegal(pos, &md);

    // En passant capture
    if (md.type == tEnPassant)
        return (md.hunter == Pawn && md.toSquare == pos->EnPassantSq());

    // Double pawn move
    if (md.type == tPawnjump)
        return IsPawnJumpLegal(pos, &md);

    // Single pawn move, including promotion
    if (md.hunter == Pawn)
        return IsPawnMoveLegal(&md);

    // Real promotion would be accepted by IsPawnMoveLegal()
    if (IsMovePromotion(move))
        return false;

    // Normal move - check square accessibility
    return (pos->AttacksFrom(md.fromSquare) & Paint(md.toSquare)) != 0;
}

// Test castling moves for pseudo-legality
bool IsCastlingLegal(Position *pos, const MoveDescription* md) {
    
    if (md->side == White && md->fromSquare == E1) {

        if (md->toSquare == G1)
            return pos->IsWhiteShortCastleLegal();

        else if (md->toSquare == C1)
            return pos->IsWhiteLongCastleLegal();
    }

    if (md->side == Black && md->fromSquare == E8) {

        if (md->toSquare == G8)
            return pos->IsBlackShortCastleLegal();

        else if (md->toSquare == C8)
            return pos->IsBlackLongCastleLegal();
    }

    return false;
}

// Test two square pawn moves for pseudo-legality
bool IsPawnJumpLegal(Position* pos, const MoveDescription *md) {
    
    // We need to test whether we are moving a pawn,
    // see comment at the top of the file. We also
    // need to enter this function in order to reject
    // moves with the wrong flag.

    if (md->hunter == Pawn &&
        md->prey == noPieceType &&
        pos->GetPiece(md->toSquare ^ 8) == noPiece) {
        if ((md->toSquare > md->fromSquare && md->side == White) ||
            (md->toSquare < md->fromSquare && md->side == Black))
            return true;
    }
    return false;
}

// Test non-jump pawn moves for pseudo-legality
bool IsPawnMoveLegal(const MoveDescription* md) {

    if (md->side == White) {

        // Missing promotion flag
        if (RankOf(md->fromSquare) == rank7 && !IsMovePromotion(md->move))
            return false;

        // Non-capture
        if (md->toSquare - md->fromSquare == 8 && md->prey == noPieceType)
           return true;
        
        // Capture
        if ((md->toSquare - md->fromSquare == 7 && FileOf(md->fromSquare) != fileA) ||
            (md->toSquare - md->fromSquare == 9 && FileOf(md->fromSquare) != fileH))
            return (md->prey != noPieceType);
 
    } else {

        // Missing promotion flag
        if (RankOf(md->fromSquare) == rank2 && !IsMovePromotion(md->move))
            return false;
        
        // Non-capture
        if (md->toSquare - md->fromSquare == -8 && md->prey == noPieceType)
            return true;
        
        // Capture
        if ((md->toSquare - md->fromSquare == -9 && FileOf(md->fromSquare) != fileA) ||
            (md->toSquare - md->fromSquare == -7 && FileOf(md->fromSquare) != fileH))
            return (md->prey != noPieceType);
    }
    return false;
}