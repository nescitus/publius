#include "types.h"
#include "piece.h"
#include "publius.h"
#include "bitboard.h"

// This file contains functions that return 
// simple information about board position

Color Position::GetSideToMove() const {
    return sideToMove;
}

int Position::GetPiece(const Square square) const {
    return pieceLocation[square];
}

int Position::Count(const Color color, const int type) const {
    return pieceCount[color][type];
}

int Position::CountMinors(const Color color) const {
    return Count(color, Knight) + Count(color, Bishop);
}

int Position::CountMajors(const Color color) const {
    return Count(color, Rook) + Count(color, Queen);
}

int Position::CountAllPawns() const {
    return Count(White, Pawn) + Count(Black, Pawn);
}

Bitboard Position::Map(const Color color, const int piece) const {
    return (pieceBitboard[color][piece]);
}

Bitboard Position::Map(const Color color) const {

    return Map(color, Pawn) | Map(color, Knight) | Map(color, Bishop)
         | Map(color, Rook) | Map(color, Queen) | Map(color, King);
}

Bitboard Position::Occupied() const {
    return Map(White) | Map(Black);
}

bool Position::IsOccupied(const Square sq) const { 
    return pieceLocation[sq] != noPiece; 
};

Bitboard Position::Empty() const {
    return ~Occupied();
}

Bitboard Position::MapDiagonalMovers(const Color color) const {
    return (Map(color, Bishop) | Map(color, Queen));
}

Bitboard Position::MapStraightMovers(const Color color) const {
    return (Map(color, Rook) | Map(color, Queen));
}

int Position::PieceTypeOnSq(const Square square) const {
    return TypeOfPiece(pieceLocation[square]);
}

Square Position::KingSq(const Color color) const {
    return kingSq[color];
}

Square Position::EnPassantSq() const {
    return enPassantSq;
}

bool Position::IsEmpty(const Square sq) const {
    return (Occupied() & Paint(sq)) == 0;
}

Bitboard Position::MapPieceType(const int pieceType) const {
    return pieceBitboard[White][pieceType] |
           pieceBitboard[Black][pieceType];
}

Bitboard Position::AllDiagMovers() const {
    return MapPieceType(Bishop) | MapPieceType(Queen);
}

Bitboard Position::AllStraightMovers() const {
    return MapPieceType(Rook) | MapPieceType(Queen);
}

bool Position::WhiteCanCastleShort() const {
    return castleFlags & wShortCastle;
}

bool Position::BlackCanCastleShort() const {
    return castleFlags & bShortCastle;
}

bool Position::WhiteCanCastleLong() const {
    return castleFlags & wLongCastle;
}

bool Position::BlackCanCastleLong() const {
    return castleFlags & bLongCastle;
}

bool Position::CanTryNullMove() const {
    return ((CountMinors(sideToMove) + CountMajors(sideToMove)) > 0);
}

bool Position::IsDraw() const {

    if (IsDrawBy50MoveRule())
        return true;

    if (IsDrawByRepetition())
        return true;

    if (IsDrawByInsufficientMaterial())
        return true;

    return false;
}

bool Position::IsDrawBy50MoveRule() const {
    return (reversibleMoves > 100);
}

bool Position::IsDrawByRepetition() const {

    for (int i = 4; i <= reversibleMoves; i += 2) {
        if (boardHash == repetitionList[repetitionIndex - i]) {
            return true;
        }
    }

    return false;
}

bool Position::IsDrawByInsufficientMaterial() const {

    if (!LeavesKingInCheck()) {
        if (CountAllPawns() + CountMajors(White) + CountMajors(Black) == 0 &&
            CountMinors(White) + CountMinors(Black) <= 1) 
            return true;
    }

    return false;
}

bool Position::IsInCheck() const {
    return (SquareIsAttacked(KingSq(sideToMove), ~sideToMove) != 0);
}

bool Position::LeavesKingInCheck() const {
    return (SquareIsAttacked(KingSq(~sideToMove), sideToMove) != 0);
}

bool Position::IsOnSq(const Color color, const int piece, const Square square) const {
    return ((Map(color, piece) & Paint(square)) != 0);
}