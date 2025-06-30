#include "types.h"
#include "bitboard.h"
#include "limits.h"
#include "publius.h"
#include "move.h"
#include "legality.h"

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