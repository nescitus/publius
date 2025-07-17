// Publius - Didactic public domain bitboard chess engine by Pawel Koziol

#include "types.h"
#include "piece.h"
#include "publius.h"
#include "bitboard.h"
#include "bitgen.h"
#include "move.h"

// This file contains functions that analyse
// and return information about board position

bool Position::SquareIsAttacked(const Square sq, const Color color) const {

    return (Map(color, Pawn) & GenerateMoves.Pawn(~color, sq)) ||
           (Map(color, Knight) & GenerateMoves.Knight(sq)) ||
           (MapDiagonalMovers(color) & GenerateMoves.Bish(Occupied(), sq)) ||
           (MapStraightMovers(color) & GenerateMoves.Rook(Occupied(), sq)) ||
           (Map(color, King) & GenerateMoves.King(sq));
}

Bitboard Position::AttacksTo(const Square sq) const {

    return (Map(White, Pawn) & GenerateMoves.Pawn(Black, sq)) |
        (Map(Black, Pawn) & GenerateMoves.Pawn(White, sq)) |
        (MapPieceType(Knight) & GenerateMoves.Knight(sq)) |
        ((AllDiagMovers()) & GenerateMoves.Bish(Occupied(), sq)) |
        ((AllStraightMovers()) & GenerateMoves.Rook(Occupied(), sq)) |
        (MapPieceType(King) & GenerateMoves.King(sq));
}

Bitboard Position::AttacksFrom(const Square sq) const {

    switch (PieceTypeOnSq(sq)) {
    case Pawn:
        return GenerateMoves.Pawn(ColorOfPiece(pieceLocation[sq]), sq);
    case Knight:
        return GenerateMoves.Knight(sq);
    case Bishop:
        return GenerateMoves.Bish(Occupied(), sq);
    case Rook:
        return GenerateMoves.Rook(Occupied(), sq);
    case Queen:
        return GenerateMoves.Queen(Occupied(), sq);
    case King:
        return GenerateMoves.King(sq);
    }
    return 0;
}

bool Position::MoveGivesCheck(const Move move) {

    Bitboard checks, occ;

    // Collect information about the move
    Color color = GetSideToMove();
    Square fromSquare = GetFromSquare(move);
    Square toSquare = GetToSquare(move);
    int hunter = PieceTypeOnSq(fromSquare);
    int prey = PieceTypeOnSq(toSquare);

    // Handle promotion
    if (IsMovePromotion(move))
        hunter = GetPromotedPiece(move);

    // Locate enemy king
    Square kingSquare = KingSq(~color);

    // Direct checks by a pawn
    if (hunter == Pawn) {
        checks = ForwardOf(SidesOf(Paint(kingSquare)), ~color);
        if (checks & Paint(toSquare)) return true;
    }

    // Init occupancy bitboard
    occ = Occupied();

    // Remove pawn in case of promotion,
    // otherwise we will not detech checks
    // along the same ray as the promoting move
    if (IsMovePromotion(move))
        occ ^= Paint(fromSquare);

    // Direct checks by a knight
    if (hunter == Knight) {
        checks = GenerateMoves.Knight(kingSquare);
        if (checks & Paint(toSquare)) return true;
    }

    // Direct diagonal checks
    if (hunter == Bishop || hunter == Queen) {
        checks = GenerateMoves.Bish(occ, kingSquare);
        if (checks & Paint(toSquare)) return true;
    }

    // Direct orthogonal checks
    if (hunter == Rook || hunter == Queen) {
        checks = GenerateMoves.Rook(occ, kingSquare);
        if (checks & Paint(toSquare)) return true;
    }

    // Prepare occupancy map after the move...
    occ = Occupied() ^ (Paint(fromSquare) | Paint(toSquare));

    // ...remembering to take captures into account
    if (prey != noPieceType)
        occ ^= Paint(toSquare);

    // Diagonal discovered checks
    checks = GenerateMoves.Bish(occ, kingSquare);
    if (checks & MapDiagonalMovers(color)) return true;

    // Orthogonal discovered checks
    checks = GenerateMoves.Rook(occ, kingSquare);
    if (checks & MapStraightMovers(color)) return true;

    // Checks discovered by en passant capture
    if (GetTypeOfMove(move) == tEnPassant) {
        if (GetSideToMove() == White)
            occ ^= Paint(toSquare - 8);
        else
            occ ^= Paint(toSquare + 8);

        checks = GenerateMoves.Bish(occ, kingSquare);
        if (checks & MapDiagonalMovers(color)) return true;

        checks = GenerateMoves.Rook(occ, kingSquare);
        if (checks & MapStraightMovers(color)) return true;
    }

    // Checks discovered by castling
    // (we make and unmake a move, as it's rare enough
    // and writing out correct conditions would be hard)
    if (GetTypeOfMove(move) == tCastle) {
        UndoData undo;
        DoMove(move, &undo);
        bool isInCheck = IsInCheck();
        UndoMove(move, &undo);
        return isInCheck;
    }

    return false;
}