#include "color.h"
#include "square.h"
#include "publius.h"
#include "bitgen.h"
#include "mask.h"
#include "hashkeys.h"
#include "piece.h"
#include "move.h"

void Position::DoMove(const int move, const int ply) {

    Color color = sideToMove;
    Square fromSquare = GetFromSquare(move);
    Square toSquare = GetToSquare(move);
    int hunter = TypeOfPiece(pieceLocation[fromSquare]);
    int prey = TypeOfPiece(pieceLocation[toSquare]);
    int moveType = GetTypeOfMove(move);

    // Save data needed for undoing a move

    undoStack[ply].move = move;
    undoStack[ply].prey = prey;
    undoStack[ply].castleFlags = castleFlags;
    undoStack[ply].enPassantSq = enPassantSq;
    undoStack[ply].reversibleMoves = reversibleMoves;
    undoStack[ply].boardHash = boardHash;
    repetitionList[repetitionIndex++] = boardHash;

    // Capture enemy piece

    if (prey != noPieceType) {
        boardHash ^= Key.pieceKey[CreatePiece(~color, prey)][toSquare];
        TakePiece(~color, prey, toSquare);
    }

    // Update reversible moves counter

    if (hunter == Pawn || prey != noPieceType) reversibleMoves = 0;
    else                                            reversibleMoves++;

    // Update castling rights

    boardHash ^= Key.castleKey[castleFlags];
    castleFlags &= castleMask[fromSquare] & castleMask[toSquare];
    boardHash ^= Key.castleKey[castleFlags];

    // Clear en passant square

    ClearEnPassant();

    // Move piece from the original square

    MovePiece(color, hunter, fromSquare, toSquare);
    boardHash ^= Key.pieceKey[CreatePiece(color, hunter)][fromSquare]
              ^ Key.pieceKey[CreatePiece(color, hunter)][toSquare];

    // Update king location

    if (hunter == King) {
        kingSq[color] = toSquare;
    }

    // Make complementary rook move in case of castling

    if (moveType == tCastle) {

        switch (toSquare) {
        case C1: { fromSquare = A1; toSquare = D1; break; }
        case G1: { fromSquare = H1; toSquare = F1; break; }
        case C8: { fromSquare = A8; toSquare = D8; break; }
        case G8: { fromSquare = H8; toSquare = F8; break; }
        }

        MovePiece(color, Rook, fromSquare, toSquare);
        boardHash ^= Key.pieceKey[CreatePiece(color, Rook)][fromSquare] ^ Key.pieceKey[CreatePiece(color, Rook)][toSquare];
    }

    // Remove pawn captured en passant

    if (moveType == tEnPassant) {
        toSquare = toSquare ^ 8;
        TakePiece(~color, Pawn, toSquare);
        boardHash ^= Key.pieceKey[CreatePiece(~color, Pawn)][toSquare];
    }

    // Set new en passant square

    if (moveType == tPawnjump) {
        toSquare = toSquare ^ 8;
        if (GenerateMoves.Pawn(color, toSquare) & (pieceBitboard[~color][Pawn])) {
            enPassantSq = toSquare;
            boardHash ^= Key.enPassantKey[FileOf(toSquare)];
        }
    }

    // Promotion

    if (IsMovePromotion(move)) {
        hunter = GetPromotedPiece(move);
        boardHash ^= Key.pieceKey[CreatePiece(color, Pawn)][toSquare]
                   ^ Key.pieceKey[CreatePiece(color, hunter)][toSquare];
        ChangePiece(Pawn, hunter, color, toSquare);
    }

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
