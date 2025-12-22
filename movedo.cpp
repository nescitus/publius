// Publius - Didactic public domain bitboard chess engine 
// by Pawel Koziol

#include "types.h"
#include "position.h"
#include "hashkeys.h"
#include "piece.h"
#include "move.h"

void Position::DoMove(const Move move, UndoData *undo) {

    // Describe move (better than loose variables)
    const MoveDescription md(*this, move);

    // Save data needed for undoing a move
    undo->move = move;
    undo->prey = md.prey;
    undo->castleFlags = castleFlags;
    undo->enPassantSq = enPassantSq;
    undo->reversibleMoves = reversibleMoves;
    undo->boardHash = boardHash;
    undo->pawnHash = pawnKingHash;

    // Update repetition list
    repetitionList[repetitionIndex++] = boardHash;

    // Capture enemy piece
    if (md.prey != noPieceType) {
        TakePiece(~md.side, md.prey, md.toSquare);
        if (md.prey == Pawn)
            pawnKingHash ^= Key.ForPiece(~md.side, md.prey, md.toSquare);
    }

    // Update reversible moves counter
    if (md.hunter == Pawn || md.prey != noPieceType) reversibleMoves = 0;
    else                                             reversibleMoves++;

    // Update pawn hash
    if (md.hunter == Pawn || md.hunter == King)
        pawnKingHash ^= Key.ForPiece(md.side, md.hunter, md.fromSquare) ^
                        Key.ForPiece(md.side, md.hunter, md.toSquare);


    // Update castling rights
    UpdateCastlingRights(md.fromSquare, md.toSquare);

    // Clear en passant square
    ClearEnPassant();

    // Move piece from the original square
    MovePiece(md.side, md.hunter, md.fromSquare, md.toSquare);

    // Update king location
    if (md.hunter == King)
        kingSq[md.side] = md.toSquare;

    // Make complementary rook move in case of castling
    if (md.type == tCastle) {
        switch (md.toSquare) {
            case C1: { MovePiece(md.side, Rook, A1, D1); break; }
            case G1: { MovePiece(md.side, Rook, H1, F1); break; }
            case C8: { MovePiece(md.side, Rook, A8, D8); break; }
            case G8: { MovePiece(md.side, Rook, H8, F8); break; }
            default: break;
        }
    }

    // Remove pawn captured en passant
    if (md.type == tEnPassant) {
        int dir = (GetSideToMove() == White ? -8 : 8);
        TakePiece(~md.side, Pawn, md.toSquare + dir);
        pawnKingHash ^= Key.ForPiece(~md.side, Pawn, md.toSquare + dir);
    }

    // Set new en passant square
    if (md.type == tPawnjump)
        SetEnPassantSquare(md.side, md.toSquare);

    // Promotion
    if (IsMovePromotion(move)) {
        const PieceType promoted = GetPromotedPiece(move);
        boardHash ^= Key.ForPiece(md.side, Pawn, md.toSquare) ^
                     Key.ForPiece(md.side, promoted, md.toSquare);
        pawnKingHash ^= Key.ForPiece(md.side, Pawn, md.toSquare);
        ChangePieceNoHash(Pawn, promoted, md.side, md.toSquare);
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
