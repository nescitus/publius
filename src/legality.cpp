// Publius - Didactic public domain bitboard chess engine by Pawel Koziol

#include "types.h"
#include "piece.h"
#include "square.h" // for rank and file
#include "bitboard.h"
#include "position.h"
#include "move.h"
#include "legality.h"

bool IsPseudoLegal(Position* pos, int move) {

    if (move == 0) 
        return false;

    Color side = pos->GetSideToMove();
    Square fromSquare = GetFromSquare(move);
    Square toSquare = GetToSquare(move);
    int hunter = pos->PieceTypeOnSq(fromSquare);
    int prey = pos->PieceTypeOnSq(toSquare);

    // from square empty or enemy piece on it
    if (hunter == noPieceType || ColorOfPiece(pos->GetPiece(fromSquare)) != side)
        return false;

    // to square empty or own piece on it
    if (prey != noPieceType && ColorOfPiece(pos->GetPiece(toSquare)) == side)
        return false;

    // castling
    if (GetTypeOfMove(move) == tCastle)
        return IsCastlingLegal(pos, side, fromSquare, toSquare);

    // en passant capture
    if (GetTypeOfMove(move) == tEnPassant)
        return (hunter == Pawn && toSquare == pos->EnPassantSq());

    // double pawn move
    if (GetTypeOfMove(move) == tPawnjump)
        return IsPawnJumpLegal(pos, side, hunter, prey, fromSquare, toSquare);

    // single pawn move, including promotion
    if (hunter == Pawn)
        return IsPawnMoveLegal(side, fromSquare, toSquare, move, hunter, prey);

    // real promotion would be accepted earlier
    if (IsMovePromotion(move))
        return false;

    // normal move - check square accessibility
    return (pos->AttacksFrom(fromSquare) & Paint(toSquare)) != 0;
}

bool IsCastlingLegal(Position *pos, Color side, Square fromSquare, Square toSquare) {
    
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

bool IsPawnJumpLegal(Position* pos, Color side, int hunter, int prey,
                     Square fromSquare, Square toSquare) {
    if (hunter == Pawn &&
        prey == noPieceType &&
        pos->GetPiece(toSquare ^ 8) == noPiece) {
        if ((toSquare > fromSquare && side == White) ||
            (toSquare < fromSquare && side == Black))
            return true;
    }
    return false;
}

bool IsPawnMoveLegal(Color side, Square fromSquare, Square toSquare, Move move, int hunter, int prey) {

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