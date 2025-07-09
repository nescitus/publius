#include "types.h"
#include "piece.h"
#include "square.h"
#include "bitboard.h"
#include "publius.h"
#include "move.h"
#include "legality.h"

bool IsPseudoLegal(Position* pos, int move) {

    if (move == 0) return false;

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
    if (GetTypeOfMove(move) == tCastle) {
        if (side == White && fromSquare == E1) {

            if (toSquare == G1) {
                if (IsWhiteShortCastleLegal(pos))
                    return true;
            } else if (toSquare == C1) {
                if (IsWhiteLongCastleLegal(pos))
                    return true;
            }
        } 
        
        if (side == Black && fromSquare == E8) {

            if (toSquare == G8) {
                if (IsBlackShortCastleLegal(pos))
                    return true;
            } else if (toSquare == C8) {
                if (IsBlackLongCastleLegal(pos))
                    return true;
            }
        }

        return false;
    }

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

bool IsWhiteShortCastleLegal(Position* pos) {

    if ((pos->WhiteCanCastleShort()) &&
        !(pos->Occupied() & Paint(F1, G1))) {

        if (!pos->SquareIsAttacked(E1, Black) &&
            !pos->SquareIsAttacked(F1, Black))
            return true;
    }

    return false;
}

bool IsWhiteLongCastleLegal(Position* pos) {

    if ((pos->WhiteCanCastleLong()) &&
        !(pos->Occupied() & Paint(B1, C1, D1))) {

        if (!pos->SquareIsAttacked(E1, Black) &&
            !pos->SquareIsAttacked(D1, Black))
            return true;
    }

    return false;
}

bool IsBlackShortCastleLegal(Position* pos) {

    if ((pos->BlackCanCastleShort()) &&
        !(pos->Occupied() & Paint(F8, G8))) {

        if (!pos->SquareIsAttacked(E8, White) &&
            !pos->SquareIsAttacked(F8, White))
            return true;
    }

    return false;
}

bool IsBlackLongCastleLegal(Position* pos) {

    if ((pos->BlackCanCastleLong()) &&
        !(pos->Occupied() & Paint(B8, C8, D8))) {

        if (!pos->SquareIsAttacked(E8, White) &&
            !pos->SquareIsAttacked(D8, White))
            return true;
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
        if (toSquare - fromSquare == 8)
            if (prey == noPieceType)
                return true;
        
        // capture
        if ((toSquare - fromSquare == 7 && FileOf(fromSquare) != fileA) ||
            (toSquare - fromSquare == 9 && FileOf(fromSquare) != fileH))
            if (prey != noPieceType)
                return true;
    } else {

        // missing promotion flag
        if (RankOf(fromSquare) == rank2 && !IsMovePromotion(move))
            return false;
        
        // non-capture
        if (toSquare - fromSquare == -8)
            if (prey == noPieceType)
                return true;
        
        // capture
        if ((toSquare - fromSquare == -9 && FileOf(fromSquare) != fileA) ||
            (toSquare - fromSquare == -7 && FileOf(fromSquare) != fileH))
            if (prey != noPieceType)
                return true;
    }
    return false;
}