// Publius - Didactic public domain bitboard chess engine 
// by Pawel Koziol

// This file contains functions that collect and return 
// information about board position

#include "types.h"
#include "piece.h"
#include "position.h"
#include "bitboard.h"
#include "bitgen.h"
#include "move.h"

bool Position::EitherSquareIsAttacked(const Square s1, const Square s2, const Color color) const {
    return SquareIsAttacked(s1, color) || SquareIsAttacked(s2, color);
}

// Detect whether a single square is attacked by given side
bool Position::SquareIsAttacked(const Square sq, const Color color) const {

    return (Map(color, Pawn) & GenerateMoves.Pawn(~color, sq)) ||
           (Map(color, Knight) & GenerateMoves.Knight(sq)) ||
           (MapDiagonalMovers(color) & GenerateMoves.Bish(Occupied(), sq)) ||
           (MapStraightMovers(color) & GenerateMoves.Rook(Occupied(), sq)) ||
           (Map(color, King) & GenerateMoves.King(sq));
}

// Return bitboard of attacks to a square (by both sides).
// Used in static exchange evaluation (badcapture.cpp).
Bitboard Position::AttacksTo(const Square sq) const {

    return (Map(White, Pawn) & GenerateMoves.Pawn(Black, sq)) |
        (Map(Black, Pawn) & GenerateMoves.Pawn(White, sq)) |
        (MapPieceType(Knight) & GenerateMoves.Knight(sq)) |
        ((AllDiagMovers()) & GenerateMoves.Bish(Occupied(), sq)) |
        ((AllStraightMovers()) & GenerateMoves.Rook(Occupied(), sq)) |
        (MapPieceType(King) & GenerateMoves.King(sq));
}

// Return bitboard of attacks from a square
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

// Detect whether a move gives check
// without making it on the board
bool Position::MoveGivesCheck(const Move move) {

    Bitboard checks, occ;

    // Collect information about the move
    const MoveDescription md(*this, move);

    // What piece is placed on the destination square?
    PieceType placedPiece = IsMovePromotion(move) ? GetPromotedPiece(move) : md.hunter;

    // Locate enemy king
    Square kingSquare = KingSq(~md.side);

    // Direct checks by a pawn
    if (placedPiece == Pawn) {
        checks = ForwardOf(SidesOf(Paint(kingSquare)), ~md.side);
        if (checks & Paint(md.toSquare)) return true;
    }

    // Init occupancy bitboard
    occ = Occupied();

    // Remove pawn in case of promotion,
    // otherwise we will not detect checks
    // along the same ray as the promoting move
    if (IsMovePromotion(move))
        occ ^= Paint(md.fromSquare);

    // Direct checks by a knight
    if (placedPiece == Knight) {
        checks = GenerateMoves.Knight(kingSquare);
        if (checks & Paint(md.toSquare)) return true;
    }

    // Direct diagonal checks
    if (placedPiece == Bishop || placedPiece == Queen) {
        checks = GenerateMoves.Bish(occ, kingSquare);
        if (checks & Paint(md.toSquare)) return true;
    }

    // Direct orthogonal checks
    if (placedPiece == Rook || placedPiece == Queen) {
        checks = GenerateMoves.Rook(occ, kingSquare);
        if (checks & Paint(md.toSquare)) return true;
    }

    // Prepare occupancy map after the move...
    occ = Occupied() ^ Paint(md.fromSquare, md.toSquare);

    // ...remembering to take captures into account
    if (md.prey != noPieceType)
        occ ^= Paint(md.toSquare);

    // Diagonal discovered checks
    checks = GenerateMoves.Bish(occ, kingSquare);
    if (checks & MapDiagonalMovers(md.side)) return true;

    // Orthogonal discovered checks
    checks = GenerateMoves.Rook(occ, kingSquare);
    if (checks & MapStraightMovers(md.side)) return true;

    // Checks discovered by en passant capture
    if (GetTypeOfMove(move) == tEnPassant) {
        int dir = (GetSideToMove() == White ? -8 : 8);
        occ ^= Paint(md.toSquare + dir);

        checks = GenerateMoves.Bish(occ, kingSquare);
        if (checks & MapDiagonalMovers(md.side)) return true;

        checks = GenerateMoves.Rook(occ, kingSquare);
        if (checks & MapStraightMovers(md.side)) return true;
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

// Detect castle legality (we are not testing
// for a check on king's destination square
// because search will handle that case)
bool Position::IsWhiteShortCastleLegal() {

    return ((castleFlags & wShortCastle) &&
           !(Occupied() & Paint(F1, G1)) &&
           !EitherSquareIsAttacked(E1, F1, Black));
}

bool Position::IsWhiteLongCastleLegal() {

    return ((castleFlags & wLongCastle) &&
           !(Occupied() & Paint(B1, C1, D1)) &&
           !EitherSquareIsAttacked(E1, D1, Black));
}

bool Position::IsBlackShortCastleLegal() {

    return ((castleFlags & bShortCastle) &&
           !(Occupied() & Paint(F8, G8)) &&
           !EitherSquareIsAttacked(E8, F8, White));
}

bool Position::IsBlackLongCastleLegal() {

    return ((castleFlags & bLongCastle) &&
           !(Occupied() & Paint(B8, C8, D8)) &&
           !EitherSquareIsAttacked(E8, D8, White));
}