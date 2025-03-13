// Publius 1.0. Didactic bitboard chess engine by Pawel Koziol

#include "color.h"
#include "square.h"
#include "limits.h"
#include "publius.h"
#include "bitboard.h"
#include "mask.h"
#include "bitgen.h"
#include "piece.h"

void AddPromotions(MoveList * list, Square fromSquare, Square toSquare) {

    list->AddMove( fromSquare, toSquare, tPromQ );
    list->AddMove( fromSquare, toSquare, tPromR );
    list->AddMove( fromSquare, toSquare, tPromB );
    list->AddMove( fromSquare, toSquare, tPromN );
}

void FillNoisyList(Position *pos, MoveList * list) {

    Bitboard pieces, moves;
    Square fromSquare, toSquare;
    Color color = pos->GetSideToMove();
    Bitboard prey = pos->Map(~color);

    if (color == White) {

        // White pawn promotions with capture (NW)

        pieces = pos->Map(White, Pawn) & Mask.rank[rank7];
        moves = NWOf(pieces) & prey;

        while (moves) {
            toSquare = PopFirstBit(&moves);
	        AddPromotions(list, toSquare - 7, toSquare);
        }

        // White pawn promotions with capture (NE)

	    moves = NEOf(pieces) & prey;
        while (moves) {
            toSquare = PopFirstBit(&moves);
	        AddPromotions(list, toSquare - 9, toSquare);
        }

        // White pawn promotions without capture

	    moves = NorthOf(pieces) & pos->Empty();
        while (moves) {
             toSquare = PopFirstBit(&moves);
	         AddPromotions(list, toSquare - 8, toSquare);
        }

	    // white pawns that cannot promote

	    pieces = pos->Map(White, Pawn) & ~Mask.rank[rank7];

        // White pawn captures (NW)

	    moves = NWOf(pieces) & prey;
        while (moves) {
            toSquare = PopFirstBit(&moves);
	        list->AddMove(toSquare - 7, toSquare, 0);
        }

        // White pawn captures (NE)

	    moves = NEOf(pieces) & prey;
        while (moves) {
            toSquare = PopFirstBit(&moves);
	        list->AddMove(toSquare - 9, toSquare, 0);
        }

        // White en passant capture

        if ((toSquare = pos->EnPassantSq() ) != sqNone) {
            if (NWOf(pieces) & Paint(toSquare))
		    list->AddMove(toSquare - 7, toSquare, tEnPassant);
            if (NEOf(pieces) & Paint(toSquare))
		    list->AddMove(toSquare - 9, toSquare, tEnPassant);
        }
    } else {

        pieces = pos->Map(Black, Pawn) & Mask.rank[rank2]; // black pawns that can promote

        // Black pawn promotions with capture (SW)

        moves = SWOf(pieces) & prey;

        while (moves) {
            toSquare = PopFirstBit(&moves);
	        AddPromotions(list, toSquare + 9, toSquare);
        }

	    // Black pawn promotions with capture (SE)

	    moves = SEOf(pieces) & prey;
        while (moves) {
            toSquare = PopFirstBit(&moves);
	        AddPromotions(list, toSquare + 7, toSquare);
        }

        // Black pawn promotions

	    moves = SouthOf(pieces) & pos->Empty();
        while (moves) {
            toSquare = PopFirstBit(&moves);
	        AddPromotions(list, toSquare + 8, toSquare);
        }

	    pieces = pos->Map(Black, Pawn) & ~Mask.rank[rank2]; // black pawns that cannot promote

        // Black pawn captures, excluding promotions (SW)

	    moves = SWOf(pieces) & prey;
        while (moves) { 
            toSquare = PopFirstBit(&moves);
	        list->AddMove(toSquare + 9, toSquare, 0);
        }

        // Black pawn captures, excluding promotions (SE)

	    moves = SEOf(pieces) & prey;
        while (moves) {
            toSquare = PopFirstBit(&moves);
	        list->AddMove(toSquare + 7, toSquare, 0);
        }

        // Black en passant capture

        if ((toSquare = pos->EnPassantSq() ) != sqNone) {
            if (SWOf(pieces) & Paint(toSquare))
		        list->AddMove(toSquare + 9, toSquare, tEnPassant);
            if (SEOf(pieces) & Paint(toSquare))
		        list->AddMove(toSquare + 7, toSquare, tEnPassant);
        }
    }

    // Captures by knight

    pieces = pos->Map(color, Knight);
    while (pieces) {
        fromSquare = PopFirstBit(&pieces);
        moves = GenerateMoves.Knight(fromSquare) & prey;
        while (moves) {
            list->AddMove(fromSquare, PopFirstBit(&moves), 0);
        }
    }

    // Diagonal captures

    pieces = pos->MapDiagonalMovers(color);
    while (pieces) {
        fromSquare = PopFirstBit(&pieces);
        moves = GenerateMoves.Bish(pos->Occupied(), fromSquare) & prey;
        while (moves) {
            list->AddMove(fromSquare, PopFirstBit(&moves), 0);
        }
    }

    // Straight line captures

    pieces = pos->MapStraightMovers(color);
    while (pieces) {
        fromSquare = PopFirstBit(&pieces);
        moves = GenerateMoves.Rook(pos->Occupied(), fromSquare) & prey;
        while (moves) {
            list->AddMove(fromSquare, PopFirstBit(&moves), 0);
        }
    }

    // Captures by king

    moves = GenerateMoves.King(pos->KingSq(color)) & prey;
    while (moves) {
        list->AddMove(pos->KingSq(color), PopFirstBit(&moves), 0);
    }

}

void FillQuietList(Position *pos, MoveList *list) {

    Bitboard pieces, moves;
    Square fromSquare, toSquare;
    Color color = pos->GetSideToMove();

    if (color == White) {

        // White short castle

        if ((pos->WhiteCanCastleShort())
        && !(pos->Occupied() & Paint(F1, G1))) {
            if (!pos->SquareIsAttacked(E1, Black)
                && !pos->SquareIsAttacked(F1, Black))
                list->AddMove(E1, G1, tCastle);
        }

        // White long castle

        if ((pos->WhiteCanCastleLong())
        && !(pos->Occupied() & Paint(B1, C1, D1))) {
            if (!pos->SquareIsAttacked(E1, Black)
                && !pos->SquareIsAttacked(D1, Black))
                list->AddMove(E1, C1, tCastle);
        }

        // White double pawn moves

        moves = (NorthOf(NorthOf(pos->Map(White, Pawn) & Mask.rank[rank2]) & pos->Empty())) & pos->Empty();
        while (moves) {
            toSquare = PopFirstBit(&moves);
	        list->AddMove(toSquare - 16, toSquare, tPawnjump);
        }

        // White normal pawn moves

        moves = NorthOf(pos->Map(White, Pawn) & ~Mask.rank[rank7]) & pos->Empty();
        while (moves) {
            toSquare = PopFirstBit(&moves);
	        list->AddMove(toSquare - 8, toSquare, 0);
        }
    } else {

        // Black short castle

        if ((pos->BlackCanCastleShort())
        && !(pos->Occupied() & Paint(F8, G8))) {
            if (!pos->SquareIsAttacked(E8, White)
                && !pos->SquareIsAttacked(F8, White))
                list->AddMove(E8, G8, tCastle);
        }

        // Black long castle

        if ((pos->BlackCanCastleLong())
        && !(pos->Occupied() & Paint(B8, C8, D8))) {
            if (!pos->SquareIsAttacked(E8, White)
                && !pos->SquareIsAttacked(D8, White))
                list->AddMove(E8, C8, tCastle);
        }

        // Black double pawn moves

        moves = (SouthOf(SouthOf(pos->Map(Black, Pawn) & Mask.rank[rank7]) & pos->Empty())) & pos->Empty();
        while (moves) {
            toSquare = PopFirstBit(&moves);
	        list->AddMove(toSquare + 16, toSquare, tPawnjump);
        }

        // Black single pawn moves

        moves = SouthOf(pos->Map(Black, Pawn) & ~Mask.rank[rank2]) & pos->Empty();
        while (moves) {
            toSquare = PopFirstBit(&moves);
	        list->AddMove(toSquare + 8, toSquare, 0);
        }
    }

    // Knight moves

    pieces = pos->Map(color, Knight);
    while (pieces) {
        fromSquare = PopFirstBit(&pieces);
        moves = GenerateMoves.Knight(fromSquare) & pos->Empty();
        while (moves) {
            list->AddMove(fromSquare, PopFirstBit(&moves), 0);
        }
    }

    // Diagonal moves

    pieces = pos->MapDiagonalMovers(color);
    while (pieces) {
        fromSquare = PopFirstBit(&pieces);
        moves = GenerateMoves.Bish(pos->Occupied(), fromSquare) & pos->Empty();
        while (moves) {
            list->AddMove(fromSquare, PopFirstBit(&moves), 0);
        }
    }

    // Straight moves

    pieces = pos->MapStraightMovers(color);
    while (pieces) {
        fromSquare = PopFirstBit(&pieces);
        moves = GenerateMoves.Rook(pos->Occupied(), fromSquare) & pos->Empty();
        while (moves) {
            list->AddMove(fromSquare, PopFirstBit(&moves), 0);
        }
    }

    // King moves

    moves = GenerateMoves.King(pos->KingSq(color)) & pos->Empty();
    while (moves) {
        list->AddMove(pos->KingSq(color), PopFirstBit(&moves), 0);
    }
}

void FillCheckList(Position* pos, MoveList* list) {

    Bitboard pieces, moves;
    Square fromSquare, toSquare;
    Color color = pos->GetSideToMove();
    Square ksq = pos->KingSq(~color);
    Bitboard knightCheck = GenerateMoves.Knight(ksq);
    Bitboard diagCheck = GenerateMoves.Bish(pos->Occupied(), ksq);
    Bitboard straightCheck = GenerateMoves.Rook(pos->Occupied(), ksq);


    /*
    if (color == White) {

        // White double pawn moves

        moves = (NorthOf(NorthOf(pos->Map(White, Pawn) & Mask.rank[rank2]) & pos->Empty())) & pos->Empty();
        while (moves) {
            toSquare = PopFirstBit(&moves);
            list->AddMove(toSquare - 16, toSquare, tPawnjump);
        }

        // White normal pawn moves

        moves = NorthOf(pos->Map(White, Pawn) & ~Mask.rank[rank7]) & pos->Empty();
        while (moves) {
            toSquare = PopFirstBit(&moves);
            list->AddMove(toSquare - 8, toSquare, 0);
        }
    }
    else {

        // Black double pawn moves

        moves = (SouthOf(SouthOf(pos->Map(Black, Pawn) & Mask.rank[rank7]) & pos->Empty())) & pos->Empty();
        while (moves) {
            toSquare = PopFirstBit(&moves);
            list->AddMove(toSquare + 16, toSquare, tPawnjump);
        }

        // Black single pawn moves

        moves = SouthOf(pos->Map(Black, Pawn) & ~Mask.rank[rank2]) & pos->Empty();
        while (moves) {
            toSquare = PopFirstBit(&moves);
            list->AddMove(toSquare + 8, toSquare, 0);
        }
    }
    */

    // Knight moves

    pieces = pos->Map(color, Knight);
    while (pieces) {
        fromSquare = PopFirstBit(&pieces);
        moves = GenerateMoves.Knight(fromSquare) & pos->Empty();
        moves &= knightCheck;
        while (moves) {
            list->AddMove(fromSquare, PopFirstBit(&moves), 0);
        }
    }

    // Diagonal moves

    pieces = pos->MapDiagonalMovers(color);
    while (pieces) {
        fromSquare = PopFirstBit(&pieces);
        moves = GenerateMoves.Bish(pos->Occupied(), fromSquare) & pos->Empty();
        moves &= diagCheck;
        while (moves) {
            list->AddMove(fromSquare, PopFirstBit(&moves), 0);
        }
    }

    // Straight moves

    pieces = pos->MapStraightMovers(color);
    while (pieces) {
        fromSquare = PopFirstBit(&pieces);
        moves = GenerateMoves.Rook(pos->Occupied(), fromSquare) & pos->Empty();
        moves &= straightCheck;
        while (moves) {
            list->AddMove(fromSquare, PopFirstBit(&moves), 0);
        }
    }

    // King moves
    /*
    moves = GenerateMoves.King(pos->KingSq(color)) & pos->Empty();
    while (moves) {
        list->AddMove(pos->KingSq(color), PopFirstBit(&moves), 0);
    }
    */
}

void FillCompleteList(Position *pos, MoveList *list) {

    FillNoisyList(pos, list);
    FillQuietList(pos, list);
}

void FillChecksAndCaptures(Position* pos, MoveList* list) {

    FillNoisyList(pos, list);
    FillCheckList(pos, list);
}