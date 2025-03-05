#include "color.h"
#include "square.h"
#include "limits.h"
#include "publius.h"
#include "move.h"
#include "piece.h"

void Position::UndoMove(const int move, const int ply) {

    Color color = ~sideToMove;
    Square fromSquare = GetFromSquare(move);
    Square toSquare = GetToSquare(move);
    int hunter = TypeOfPiece(pieceLocation[toSquare]);
    int prey = undoStack[ply].prey;
    int moveType = GetTypeOfMove(move);

    castleFlags = undoStack[ply].castleFlags;
    enPassantSq = undoStack[ply].enPassantSq;
    reversibleMoves = undoStack[ply].reversibleMoves;
    boardHash = undoStack[ply].boardHash;
    repetitionIndex--;

    MovePieceNoHash(color, hunter, toSquare, fromSquare);

    // Update king location

    if (hunter == King) {
        kingSq[color] = fromSquare;
    }

    // Undo capture

    if (prey != noPieceType) {
        AddPiece(~color, prey, toSquare);
    }

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

    if (moveType == tEnPassant) {
        AddPiece(~color, Pawn, toSquare ^ 8);
    }

    // Change promoted piece back to pawn

    if (IsMovePromotion(move)) {
        ChangePiece(hunter, Pawn, color, fromSquare);
    }

    // Switch side

    sideToMove = ~sideToMove;
}

void Position::UndoNull(const int ply) {

    enPassantSq = undoStack[ply].enPassantSq;
    boardHash = undoStack[ply].boardHash;
    repetitionIndex--;
    reversibleMoves--;
    sideToMove = ~sideToMove;
}
