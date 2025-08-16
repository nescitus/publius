// Publius - Didactic public domain bitboard chess engine 
// by Pawel Koziol

// This file contains functions that return 
// simple information about board position.
// Even more basic getters, like Count(), are
// defined in position.h

#include "types.h"
#include "piece.h"
#include "position.h"
#include "bitboard.h"

int Position::CountMinors(const Color color) const {
    return Count(color, Knight) + Count(color, Bishop);
}

int Position::CountMajors(const Color color) const {
    return Count(color, Rook) + Count(color, Queen);
}

int Position::CountAllPawns() const {
    return Count(White, Pawn) + Count(Black, Pawn);
}

Bitboard Position::MapColor(const Color color) const {

    return Map(color, Pawn) | Map(color, Knight) | Map(color, Bishop)
         | Map(color, Rook) | Map(color, Queen) | Map(color, King);
}

Bitboard Position::Occupied() const {
    return MapColor(White) | MapColor(Black);
}

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

// Requires side to move to have at least one non-pawn,
// reducing the risk of making null move in zugzwang positions.
// Other conditions for null move are implemented in search
bool Position::CanTryNullMove() const {
    return ((CountMinors(sideToMove) + CountMajors(sideToMove)) > 0);
}

// Is position drawn? NOTE: c++ enforces strict
// left to right order of evaluating expressions,
// and we start with cheaper tests.
bool Position::IsDraw() const {

    return (reversibleMoves >= 100) || 
            IsDrawByRepetition()||
            IsDrawByInsufficientMaterial();
}

bool Position::IsDrawByRepetition() const {

    for (int i = 4; i <= reversibleMoves; i += 2) {
        if (boardHash == repetitionList[repetitionIndex - i])
            return true;
    }

    return false;
}

bool Position::IsDrawByInsufficientMaterial() const {

    if (!IsOwnKingInCheck()) {
        if (CountAllPawns() + CountMajors(White) + CountMajors(Black) == 0 &&
            CountMinors(White) + CountMinors(Black) <= 1) 
            return true;
    }

    return false;
}

bool Position::IsInCheck() const {
    return (SquareIsAttacked(KingSq(sideToMove), ~sideToMove) != 0);
}

bool Position::IsOwnKingInCheck() const {
    return (SquareIsAttacked(KingSq(~sideToMove), sideToMove) != 0);
}