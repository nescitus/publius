#include "color.h"
#include "square.h"
#include "limits.h"
#include "publius.h"
#include "bitgen.h"
#include "mask.h"
#include "hashkeys.h"
#include "piece.h"
#include "move.h"

void Position::DoMove(const Move move, const int ply) {

    // Init variables
    const Color color = sideToMove;
    const Square fromSquare = GetFromSquare(move);
    const Square toSquare = GetToSquare(move);
    const int hunter = TypeOfPiece(pieceLocation[fromSquare]);
    const int prey = TypeOfPiece(pieceLocation[toSquare]);
    const int moveType = GetTypeOfMove(move);

    // Save data needed for undoing a move
    undoStack[ply].move = move;
    undoStack[ply].prey = prey;
    undoStack[ply].castleFlags = castleFlags;
    undoStack[ply].enPassantSq = enPassantSq;
    undoStack[ply].reversibleMoves = reversibleMoves;
    undoStack[ply].boardHash = boardHash;
    undoStack[ply].pawnHash = pawnHash;

    // Update repetition list
    repetitionList[repetitionIndex++] = boardHash;

    // Capture enemy piece
    if (prey != noPieceType) {
        TakePiece(~color, prey, toSquare);
        if (prey == Pawn)
            pawnHash ^= Key.pieceKey[CreatePiece(~color, prey)][toSquare];
    }

    // Update reversible moves counter
    if (hunter == Pawn || prey != noPieceType) reversibleMoves = 0;
    else                                       reversibleMoves++;

    // Update pawn hash
    if (hunter == Pawn || hunter == King)
        pawnHash ^= Key.pieceKey[CreatePiece(color, hunter)][fromSquare] 
                  ^ Key.pieceKey[CreatePiece(color, hunter)][toSquare];


    // Update castling rights
    UpdateCastlingRights(fromSquare, toSquare);

    // Clear en passant square
    ClearEnPassant();

    // Move piece from the original square
    MovePiece(color, hunter, fromSquare, toSquare);

    // Update king location
    if (hunter == King) {
        kingSq[color] = toSquare;
    }

    // Make complementary rook move in case of castling
    if (moveType == tCastle) {
        switch (toSquare) {
            case C1: { MovePiece(color, Rook, A1, D1); break; }
            case G1: { MovePiece(color, Rook, H1, F1); break; }
            case C8: { MovePiece(color, Rook, A8, D8); break; }
            case G8: { MovePiece(color, Rook, H8, F8); break; }
        }
    }

    // Remove pawn captured en passant
    if (moveType == tEnPassant) {
        TakePiece(~color, Pawn, toSquare ^ 8);
        pawnHash ^= Key.pieceKey[CreatePiece(~color, Pawn)][toSquare ^ 8];
    }

    // Set new en passant square
    if (moveType == tPawnjump) {
        SetEnPassantSquare(color, toSquare);
    }

    // Promotion
    if (IsMovePromotion(move)) {
        const int promoted = GetPromotedPiece(move);
        boardHash ^= Key.pieceKey[CreatePiece(color, Pawn)][toSquare]
                   ^ Key.pieceKey[CreatePiece(color, promoted)][toSquare];
        pawnHash ^= Key.pieceKey[CreatePiece(color, Pawn)][toSquare];
        ChangePieceNoHash(Pawn, promoted, color, toSquare);
    }

    // Switch side to move
    SwitchSide();

}

void Position::DoNull(const int ply) {

    // Save stuff
    undoStack[ply].move = 0;
    undoStack[ply].enPassantSq = enPassantSq;
    undoStack[ply].boardHash = boardHash;

    // Update repetition list
    repetitionList[repetitionIndex++] = boardHash;
    reversibleMoves++;

    ClearEnPassant();
    SwitchSide();
}