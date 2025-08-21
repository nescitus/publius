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

// Map squares occupied by pieces of a given color
Bitboard Position::Pieces(const Color color) const {

    return Map(color, Pawn) | Map(color, Knight) | Map(color, Bishop)
         | Map(color, Rook) | Map(color, Queen) | Map(color, King);
}

// Map occupied squares
Bitboard Position::Occupied() const {
    return Pieces(White) | Pieces(Black);
}

// Map empty squares
Bitboard Position::Empty() const {
    return ~Occupied();
}

// Map pieces of a given color that move diagonally
Bitboard Position::MapDiagonalMovers(const Color color) const {
    return (Map(color, Bishop) | Map(color, Queen));
}

// Map pieces of a given color that move vertically 
// and horizontally
Bitboard Position::MapStraightMovers(const Color color) const {
    return (Map(color, Rook) | Map(color, Queen));
}

// Get type of the piece occupying given square
int Position::PieceTypeOnSq(const Square square) const {
    return TypeOfPiece(pieceLocation[square]);
}

// Map pieces that move diagonally (both colors)
Bitboard Position::AllDiagMovers() const {
    return MapPieceType(Bishop) | MapPieceType(Queen);
}

// Map pieces that move vertically and horizontally
// (both colors)
Bitboard Position::AllStraightMovers() const {
    return MapPieceType(Rook) | MapPieceType(Queen);
}

// Requires side to move to have at least one non-pawn,
// reducing the risk of making null move in zugzwang positions.
// Other conditions for null move are implemented in search
bool Position::CanTryNullMove() const {
    return ((CountMinors(sideToMove) + CountMajors(sideToMove)) > 0);
}

// Is position drawn? NOTE: C++ guarantees left-to-right 
// evaluation and short-circuit for ||, so we put cheaper 
// checks first.
bool Position::IsDraw() const {

    return (reversibleMoves >= 100) || 
            IsDrawByRepetition()||
            IsDrawByInsufficientMaterial();
}

// Detect draw by repetition
bool Position::IsDrawByRepetition() const {

    for (int i = 4; i <= reversibleMoves; i += 2) {
        if (boardHash == repetitionList[repetitionIndex - i])
            return true;
    }

    return false;
}

// Detect draw by insufficient material
bool Position::IsDrawByInsufficientMaterial() const {

    if (!IsOwnKingInCheck()) {
        if (CountAllPawns() + CountMajors(White) + CountMajors(Black) == 0 &&
            CountMinors(White) + CountMinors(Black) <= 1) 
            return true;
    }

    return false;
}

// Is side to move in check?
bool Position::IsInCheck() const {
    return SquareIsAttacked(KingSq(sideToMove), ~sideToMove);
}

// Is king of the side not to move in check?
// If so, we have made an illegal move.
bool Position::IsOwnKingInCheck() const {
    return SquareIsAttacked(KingSq(~sideToMove), sideToMove);
}