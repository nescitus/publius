// Publius - Didactic public domain bitboard chess engine by Pawel Koziol

#include "types.h"
#include "position.h"
#include "publius.h"
#include "bitboard.h"
#include "mask.h"
#include "bitgen.h"
#include "legality.h"
#include "gen.h"

// Non-pawn moves are generated on a per-piece basis.
// Knowing piece location (fromSquare) and bitboard
// of its move target, we fill the move list one 
// toSquare at a time.

void SerializeMoves(MoveList* list, const Square fromSquare, Bitboard moves) {
    
    while (moves)
        list->AddMove(fromSquare, PopFirstBit(&moves), 0);
}

// Pawn move generation is done in bulk: we generate 
// all the target squares for pawn moves in a specific 
// direction (straight moves or diagonal captures).
// Knowing the move direction, we infer fromSquare 
// from toSquare and move vector.

void SerializePawnMoves(MoveList* list, Bitboard moves, int vector, int flag) {
    
    Square fromSquare, toSquare;
    
    while (moves) {
        toSquare = PopFirstBit(&moves);
        fromSquare = toSquare + vector;
        list->AddMove(fromSquare, toSquare, flag);
    }
}

// Promotions are serialized very much like pawn moves

void SerializePromotions(MoveList* list, Bitboard moves, int vector) {

    Square fromSquare, toSquare;

    while (moves) {
        toSquare = PopFirstBit(&moves);
        fromSquare = toSquare + vector;

        list->AddMove(fromSquare, toSquare, tPromQ);
        list->AddMove(fromSquare, toSquare, tPromR);
        list->AddMove(fromSquare, toSquare, tPromB);
        list->AddMove(fromSquare, toSquare, tPromN);
    }
}

// Fill move list with moves that change material balance:
// captures, including en passant, and promotions

void FillNoisyList(Position* pos, MoveList* list) {

    Bitboard pieces, moves;
    Square fromSquare, toSquare;
    Color color = pos->GetSideToMove();
    Bitboard prey = pos->MapColor(~color);

    if (color == White) {

        // White pawn promotions with capture (NW)
        pieces = pos->Map(White, Pawn) & Mask.rank[rank7];
        moves = NWOf(pieces) & prey;
        SerializePromotions(list, moves, -7);

        // White pawn promotions with capture (NE)
        moves = NEOf(pieces) & prey;
        SerializePromotions(list, moves, -9);

        // White pawn promotions without capture
        moves = NorthOf(pieces) & pos->Empty();
        SerializePromotions(list, moves, -8);

        // Map white pawns that cannot promote
        pieces = pos->Map(White, Pawn) & ~Mask.rank[rank7];

        // White pawn captures (NW)
        moves = NWOf(pieces) & prey;
        SerializePawnMoves(list, moves, -7, tNormal);

        // White pawn captures (NE)
        moves = NEOf(pieces) & prey;
        SerializePawnMoves(list, moves, -9, tNormal);

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
        SerializePromotions(list, moves, 9);

        // Black pawn promotions with capture (SE)
        moves = SEOf(pieces) & prey;
        SerializePromotions(list, moves, 7);

        // Black pawn promotions
        moves = SouthOf(pieces) & pos->Empty();
        SerializePromotions(list, moves, 8);

        // Map black pawns that cannot promote
        pieces = pos->Map(Black, Pawn) & ~Mask.rank[rank2];

        // Black pawn captures, excluding promotions (SW)
        moves = SWOf(pieces) & prey;
        SerializePawnMoves(list, moves, 9, tNormal);

        // Black pawn captures, excluding promotions (SE)
        moves = SEOf(pieces) & prey;
        SerializePawnMoves(list, moves, 7, tNormal);

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
        SerializeMoves(list, fromSquare, moves);
    }

    // Diagonal captures
    pieces = pos->MapDiagonalMovers(color);
    while (pieces) {
        fromSquare = PopFirstBit(&pieces);
        moves = GenerateMoves.Bish(pos->Occupied(), fromSquare) & prey;
        SerializeMoves(list, fromSquare, moves);
    }

    // Straight line captures
    pieces = pos->MapStraightMovers(color);
    while (pieces) {
        fromSquare = PopFirstBit(&pieces);
        moves = GenerateMoves.Rook(pos->Occupied(), fromSquare) & prey;
        SerializeMoves(list, fromSquare, moves);
    }

    // Captures by king
    moves = GenerateMoves.King(pos->KingSq(color)) & prey;
    SerializeMoves(list, pos->KingSq(color), moves);
}

void FillQuietList(Position* pos, MoveList* list) {

    Bitboard pieces, moves;
    Square fromSquare, toSquare;
    Color color = pos->GetSideToMove();

    if (color == White) {

        // White short castle
        if (pos->IsWhiteShortCastleLegal())
            list->AddMove(E1, G1, tCastle);
        
        // White long castle
        if (pos->IsWhiteLongCastleLegal())
            list->AddMove(E1, C1, tCastle);

        // White double pawn moves
        moves = (NorthOf(NorthOf(pos->Map(White, Pawn) & Mask.rank[rank2]) & pos->Empty())) & pos->Empty();
        SerializePawnMoves(list, moves, -16, tPawnjump);

        // White normal pawn moves
        moves = NorthOf(pos->Map(White, Pawn) & ~Mask.rank[rank7]) & pos->Empty();
        SerializePawnMoves(list, moves, -8, tNormal);

    } else {
 
        // Black short castle
        if (pos->IsBlackShortCastleLegal())
           list->AddMove(E8, G8, tCastle);
        
        // Black long castle
        if (pos->IsBlackLongCastleLegal())
           list->AddMove(E8, C8, tCastle);
        
        // Black double pawn moves
        moves = (SouthOf(SouthOf(pos->Map(Black, Pawn) & Mask.rank[rank7]) & pos->Empty())) & pos->Empty();
        SerializePawnMoves(list, moves, 16, tPawnjump);

        // Black single pawn moves
        moves = SouthOf(pos->Map(Black, Pawn) & ~Mask.rank[rank2]) & pos->Empty();
        SerializePawnMoves(list, moves, 8, tNormal);
    }

    // Knight moves
    pieces = pos->Map(color, Knight);
    while (pieces) {
        fromSquare = PopFirstBit(&pieces);
        moves = GenerateMoves.Knight(fromSquare) & pos->Empty();
        SerializeMoves(list, fromSquare, moves);
    }

    // Diagonal moves
    pieces = pos->MapDiagonalMovers(color);
    while (pieces) {
        fromSquare = PopFirstBit(&pieces);
        moves = GenerateMoves.Bish(pos->Occupied(), fromSquare) & pos->Empty();
        SerializeMoves(list, fromSquare, moves);
    }

    // Straight moves
    pieces = pos->MapStraightMovers(color);
    while (pieces) {
        fromSquare = PopFirstBit(&pieces);
        moves = GenerateMoves.Rook(pos->Occupied(), fromSquare) & pos->Empty();
        SerializeMoves(list, fromSquare, moves);
    }

    // King moves
    moves = GenerateMoves.King(pos->KingSq(color)) & pos->Empty();
    SerializeMoves(list, pos->KingSq(color), moves);
}

// So far, we are generating only direct checks here.
// Adding discovered checks requires detecting whether
// a moving piece is between the king and a potential
// checker and is not moving along the same ray.
// This will be done at some stage, but is not
// the top priority right now.

void FillCheckList(Position* pos, MoveList* list) {

    Bitboard pieces, moves;
    Square fromSquare;
    Color color = pos->GetSideToMove();
    Square kingSquare = pos->KingSq(~color);
    
    // find locations from where the direct checks can be given
    Bitboard pawnChecks = ForwardOf(SidesOf(Paint(kingSquare)), ~color);
    Bitboard knightCheck = GenerateMoves.Knight(kingSquare);
    Bitboard diagCheck = GenerateMoves.Bish(pos->Occupied(), kingSquare);
    Bitboard straightCheck = GenerateMoves.Rook(pos->Occupied(), kingSquare);
    
    if (color == White) {

        // White double pawn moves
        moves = (NorthOf(NorthOf(pos->Map(White, Pawn) & Mask.rank[rank2]) & pos->Empty())) & pos->Empty();
        moves &= pawnChecks;
        SerializePawnMoves(list, moves, -16, tPawnjump);

        // White single pawn moves
        moves = NorthOf(pos->Map(White, Pawn) & ~Mask.rank[rank7]) & pos->Empty();
        moves &= pawnChecks;
        SerializePawnMoves(list, moves, -8, tNormal);
    }
    else {

        // Black double pawn moves
        moves = (SouthOf(SouthOf(pos->Map(Black, Pawn) & Mask.rank[rank7]) & pos->Empty())) & pos->Empty();
        moves &= pawnChecks;
        SerializePawnMoves(list, moves, 16, tPawnjump);

        // Black single pawn moves
        moves = SouthOf(pos->Map(Black, Pawn) & ~Mask.rank[rank2]) & pos->Empty();
        moves &= pawnChecks;
        SerializePawnMoves(list, moves, 8, tNormal);
    }

    // Knight moves
    pieces = pos->Map(color, Knight);
    while (pieces) {
        fromSquare = PopFirstBit(&pieces);
        moves = GenerateMoves.Knight(fromSquare) & pos->Empty();
        moves &= knightCheck;
        SerializeMoves(list, fromSquare, moves);
    }

    // Diagonal moves
    pieces = pos->MapDiagonalMovers(color);
    while (pieces) {
        fromSquare = PopFirstBit(&pieces);
        moves = GenerateMoves.Bish(pos->Occupied(), fromSquare) & pos->Empty();
        moves &= diagCheck;
        SerializeMoves(list, fromSquare, moves);
    }

    // Straight moves
    pieces = pos->MapStraightMovers(color);
    while (pieces) {
        fromSquare = PopFirstBit(&pieces);
        moves = GenerateMoves.Rook(pos->Occupied(), fromSquare) & pos->Empty();
        moves &= straightCheck;
        SerializeMoves(list, fromSquare, moves);
    }

    // King moves
    /*
    moves = GenerateMoves.King(pos->KingSq(color)) & pos->Empty();
    while (moves) {
        list->AddMove(pos->KingSq(color), PopFirstBit(&moves), 0);
    }
    */
}