#include "types.h"
#include "publius.h"
#include "move.h"
#include "piece.h"

void Position::UndoMove(const Move move, UndoData* undo) {

    // Init variables
    const Color color = ~sideToMove;
    const Square fromSquare = GetFromSquare(move);
    const Square toSquare = GetToSquare(move);
    const int hunter = TypeOfPiece(pieceLocation[toSquare]);
    const int prey = undo->prey;
    const int moveType = GetTypeOfMove(move);

    // Copy stuff needed to undo the move
    castleFlags = undo->castleFlags;
    enPassantSq = undo->enPassantSq;
    reversibleMoves = undo->reversibleMoves;
    boardHash = undo->boardHash;
    pawnHash = undo->pawnHash;
    repetitionIndex--;

    // Move piece
    MovePieceNoHash(color, hunter, toSquare, fromSquare);

    // Update king location
    if (hunter == King)
        kingSq[color] = fromSquare;

    // Undo capture
    if (prey != noPieceType)
        AddPieceNoHash(~color, prey, toSquare);

    // Undo complementary rook move in case of castling
    if (moveType == tCastle) {
        switch (toSquare) {
        case C1: { MovePieceNoHash(color, Rook, D1, A1); break; }
        case G1: { MovePieceNoHash(color, Rook, F1, H1); break; }
        case C8: { MovePieceNoHash(color, Rook, D8, A8); break; }
        case G8: { MovePieceNoHash(color, Rook, F8, H8); break; }
        }
    }

    // Reinstate a pawn captured en passant
    if (moveType == tEnPassant)
        AddPieceNoHash(~color, Pawn, toSquare ^ 8);

    // Change promoted piece back to pawn
    if (IsMovePromotion(move))
        ChangePieceNoHash(hunter, Pawn, color, fromSquare);

    // Switch side (we don't use SwitchSide() function,
    // as it would modify the hash key)
    sideToMove = ~sideToMove;
}

void Position::UndoNull(UndoData* undo) {

    enPassantSq = undo->enPassantSq;
    boardHash = undo->boardHash;
    repetitionIndex--;
    reversibleMoves--;
    sideToMove = ~sideToMove;
}
