// Publius - Didactic public domain bitboard chess engine 
// by Pawel Koziol

// This file contains functions detecting
// whether a move is pseudo-legal

#include "types.h"
#include "piece.h"
#include "square.h" // for rank and file
#include "bitboard.h"
#include "position.h"
#include "move.h"
#include "legality.h"
#include "publius.h"
#include <iostream>

bool IsPseudoLegal(Position* pos, int move) {

    // NOTE: move is not guaranteed to work in the current
    // position, this is why we are testing it in the first
    // place. The less obvious implication is that we cannot 
    // trust the move flag. For example, you can have a legal
    // d2d4 move by a queen and compare it with an input move 
    // flagged as a pawn jump. I learned it the hard way during 
    // an unsuccessful refactor.

    if (move == 0) 
        return false;

    const Color side = pos->GetSideToMove();
    const Square fromSquare = GetFromSquare(move);
    const Square toSquare = GetToSquare(move);
    const PieceType hunterType = pos->PieceTypeOnSq(fromSquare);
    const PieceType preyType = pos->PieceTypeOnSq(toSquare);

    // from square empty or enemy piece on it
    if (hunterType == noPieceType || 
        ColorOfPiece(pos->GetPiece(fromSquare)) != side)
        return false;

    // to square empty or own piece on it
    if (preyType != noPieceType && 
        ColorOfPiece(pos->GetPiece(toSquare)) == side)
        return false;       

    // castling
    if (GetTypeOfMove(move) == tCastle)
        return IsCastlingLegal(pos, side, fromSquare, toSquare);

    // en passant capture
    if (GetTypeOfMove(move) == tEnPassant)
        return (hunterType == Pawn && toSquare == pos->EnPassantSq());

    // double pawn move
    if (GetTypeOfMove(move) == tPawnjump)
        return IsPawnJumpLegal(pos, side, hunterType, preyType, fromSquare, toSquare);

    // single pawn move, including promotion
    if (hunterType == Pawn)
        return IsPawnMoveLegal(side, fromSquare, toSquare, move, preyType);

    // real promotion would be accepted by IsPawnMoveLegal()
    if (IsMovePromotion(move))
        return false;

    // normal move - check square accessibility
    return (pos->AttacksFrom(fromSquare) & Paint(toSquare)) != 0;
}

bool IsCastlingLegal(Position *pos, const Color side, const Square fromSquare, const Square toSquare) {
    
    if (side == White && fromSquare == E1) {

        if (toSquare == G1)
            return pos->IsWhiteShortCastleLegal();

        else if (toSquare == C1)
            return pos->IsWhiteLongCastleLegal();
    }

    if (side == Black && fromSquare == E8) {

        if (toSquare == G8)
            return pos->IsBlackShortCastleLegal();

        else if (toSquare == C8)
            return pos->IsBlackLongCastleLegal();
    }

    return false;
}

bool IsPawnJumpLegal(Position* pos, const Color side, const PieceType hunter, const PieceType prey,
                     const Square fromSquare, const Square toSquare) {
    
    // We need to test whether we are moving a pawn,
    // see comment at the top of the file. We also
    // need to enter this function in order to reject
    // moves with the wring flag.

    if (hunter == Pawn &&
        prey == noPieceType &&
        pos->GetPiece(toSquare ^ 8) == noPiece) {
        if ((toSquare > fromSquare && side == White) ||
            (toSquare < fromSquare && side == Black))
            return true;
    }
    return false;
}

bool IsPawnMoveLegal(const Color side, const Square fromSquare, const Square toSquare, 
                     const Move move, const PieceType prey) {

    if (side == White) {

        // missing promotion flag
        if (RankOf(fromSquare) == rank7 && !IsMovePromotion(move))
            return false;

        // non-capture
        if (toSquare - fromSquare == 8 && prey == noPieceType)
           return true;
        
        // capture
        if ((toSquare - fromSquare == 7 && FileOf(fromSquare) != fileA) ||
            (toSquare - fromSquare == 9 && FileOf(fromSquare) != fileH))
            return (prey != noPieceType);
 
    } else {

        // missing promotion flag
        if (RankOf(fromSquare) == rank2 && !IsMovePromotion(move))
            return false;
        
        // non-capture
        if (toSquare - fromSquare == -8 && prey == noPieceType)
            return true;
        
        // capture
        if ((toSquare - fromSquare == -9 && FileOf(fromSquare) != fileA) ||
            (toSquare - fromSquare == -7 && FileOf(fromSquare) != fileH))
            return (prey != noPieceType);
    }
    return false;
}