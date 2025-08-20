// Publius - Didactic public domain bitboard chess engine by Pawel Koziol

#include "types.h"
#include "position.h"
#include "bitgen.h"
#include "mask.h"
#include "hashkeys.h"
#include "piece.h"
#include "move.h"

void Position::DoMove(const Move move, UndoData *undo) {

    // Init variables
    const Color color = sideToMove;
    const Square fromSquare = GetFromSquare(move);
    const Square toSquare = GetToSquare(move);
    const int hunter = TypeOfPiece(pieceLocation[fromSquare]);
    const int prey = TypeOfPiece(pieceLocation[toSquare]);
    const int moveType = GetTypeOfMove(move);

    // Save data needed for undoing a move
    undo->move = move;
    undo->prey = prey;
    undo->castleFlags = castleFlags;
    undo->enPassantSq = enPassantSq;
    undo->reversibleMoves = reversibleMoves;
    undo->boardHash = boardHash;
    undo->pawnHash = pawnKingHash;

    // Update repetition list
    repetitionList[repetitionIndex++] = boardHash;

    // Capture enemy piece
    if (prey != noPieceType) {
        TakePiece(~color, prey, toSquare);
        if (prey == Pawn)
            pawnKingHash ^= Key.ForPiece(~color, prey, toSquare);
    }

    // Update reversible moves counter
    if (hunter == Pawn || prey != noPieceType) reversibleMoves = 0;
    else                                       reversibleMoves++;

    // Update pawn hash
    if (hunter == Pawn || hunter == King)
        pawnKingHash ^= Key.ForPiece(color, hunter, fromSquare) ^
                        Key.ForPiece(color, hunter, toSquare);


    // Update castling rights
    UpdateCastlingRights(fromSquare, toSquare);

    // Clear en passant square
    ClearEnPassant();

    // Move piece from the original square
    MovePiece(color, hunter, fromSquare, toSquare);

    // Update king location
    if (hunter == King)
        kingSq[color] = toSquare;

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
        int dir = (GetSideToMove() == White ? -8 : 8);
        TakePiece(~color, Pawn, toSquare + dir);
        pawnKingHash ^= Key.ForPiece(~color, Pawn, toSquare + dir);
    }

    // Set new en passant square
    if (moveType == tPawnjump)
        SetEnPassantSquare(color, toSquare);

    // Promotion
    if (IsMovePromotion(move)) {
        const int promoted = GetPromotedPiece(move);
        boardHash ^= Key.ForPiece(color, Pawn, toSquare) ^
                     Key.ForPiece(color, promoted, toSquare);
        pawnKingHash ^= Key.ForPiece(color, Pawn, toSquare);
        ChangePieceNoHash(Pawn, promoted, color, toSquare);
    }

    // Switch side to move and update hash key
    sideToMove = ~sideToMove;
    boardHash ^= sideRandom;
}

void Position::DoNull(UndoData* undo) {

    // Save stuff
    undo->move = 0;
    undo->enPassantSq = enPassantSq;
    undo->boardHash = boardHash;

    // Update repetition list
    repetitionList[repetitionIndex++] = boardHash;
    reversibleMoves++;

    ClearEnPassant();
    sideToMove = ~sideToMove;
    boardHash ^= sideRandom;
}