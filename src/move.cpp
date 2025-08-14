// Publius - Didactic public domain bitboard chess engine by Pawel Koziol

#include "types.h"
#include "square.h"
#include "position.h"
#include "move.h"
#include "piece.h"

int GetTypeOfMove(const Move move) {
    return move >> 12;
}

Square GetFromSquare(const Move move) {
    return static_cast<Square>(move & 63);
}

Square GetToSquare(const Move move) {
    return static_cast<Square>((move >> 6) & 63);
}

int GetPromotedPiece(const Move move) {
    return (move >> 12) - 3;
}

Move CreateMove(const Square fromSquare, 
               const Square toSquare, 
               const int flag) {
    return (flag << 12) | (toSquare << 6) | fromSquare;
}

bool IsMovePromotion(const Move move) {
    return move & 0x4000;
}

// does this move change material balance?
bool IsMoveNoisy(Position* pos, const Move move) {

    return (pos->GetPiece(GetToSquare(move)) != noPiece ||
           IsMovePromotion(move) ||
           GetTypeOfMove(move) == tEnPassant);
}

std::string MoveToString(const Move move) {

    static const char prom_char[5] = "nbrq";
    std::string move_str = SquareName(GetFromSquare(move))
                         + SquareName(GetToSquare(move));

    if (IsMovePromotion(move))
        move_str += prom_char[(move >> 12) & 3];

    return move_str;
}

Move StringToMove(Position* pos, const std::string& moveString) {

    Square fromSquare, toSquare;
    int moveFlag;
    std::string moveStr;
    moveStr = moveString;

    fromSquare = MakeSquare(moveStr[0] - 'a', moveStr[1] - '1');
    toSquare = MakeSquare(moveStr[2] - 'a', moveStr[3] - '1');
    moveFlag = tNormal;

    // a king moves by two squares - must be castling
    if (pos->PieceTypeOnSq(fromSquare) == King && 
        AbsoluteDelta(toSquare, fromSquare) == 2) {
        moveFlag = tCastle;
    }

    // there are many special pawn moves:
    else if (pos->PieceTypeOnSq(fromSquare) == Pawn) {
        // en passant capture
        if (toSquare == pos->EnPassantSq())
            moveFlag = tEnPassant;

        // double move whuch might set en passant square
        else if (AbsoluteDelta(toSquare, fromSquare) == 16)
            moveFlag = tPawnjump;
       
        // promotion
        else if (moveStr.length() > 4 && moveStr[4] != '\0') {
            switch (moveStr[4]) {
            case 'n':
                moveFlag = tPromN;
                break;
            case 'b':
                moveFlag = tPromB;
                break;
            case 'r':
                moveFlag = tPromR;
                break;
            case 'q':
                moveFlag = tPromQ;
                break;
            }
        }
    }

    return CreateMove(fromSquare, toSquare, moveFlag);
}

