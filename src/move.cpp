#include "types.h"
#include "square.h"
#include "publius.h"
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

    Square from, to;
    int type;
    std::string move_str;
    move_str = moveString;

    from = MakeSquare(move_str[0] - 'a', move_str[1] - '1');
    to = MakeSquare(move_str[2] - 'a', move_str[3] - '1');
    type = tNormal;

    // a king moves by two squares - must be castling
    if (pos->PieceTypeOnSq(from) == King && 
        AbsoluteDelta(to, from) == 2) {
        type = tCastle;
    }

    // there are many special pawn moves:
    else if (pos->PieceTypeOnSq(from) == Pawn) {
        // en passant capture
        if (to == pos->EnPassantSq()) {
            type = tEnPassant;
        }
        // double move whuch might set en passant square
        else if (AbsoluteDelta(to, from) == 16) {
            type = tPawnjump;
        }
        // promotion
        else if (move_str.length() > 4 && move_str[4] != '\0') {
            switch (move_str[4]) {
            case 'n':
                type = tPromN;
                break;
            case 'b':
                type = tPromB;
                break;
            case 'r':
                type = tPromR;
                break;
            case 'q':
                type = tPromQ;
                break;
            }
        }
    }

    return CreateMove(from, to, type);
}

