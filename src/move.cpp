#include "color.h"
#include "square.h"
#include "limits.h"
#include "publius.h"
#include "move.h"
#include "piece.h"

int GetTypeOfMove(const int move) {
    return move >> 12;
}

Square GetFromSquare(const int move) {
    return static_cast<Square>(move & 63);
}

Square GetToSquare(const int move) {
    return static_cast<Square>((move >> 6) & 63);
}

int GetPromotedPiece(const int move) {
	return (move >> 12) - 3;
}

int CreateMove(const Square fromSquare, 
               const Square toSquare, 
               const int flag) {
    return (flag << 12) | (toSquare << 6) | fromSquare;
}

bool IsMovePromotion(const int move) {
	return move & 0x4000;
}

// does this move change material balance?
bool IsMoveNoisy(Position *pos, const int move) {

    return (pos->GetPiece(GetToSquare(move)) != noPiece
        || IsMovePromotion(move)
        || GetTypeOfMove(move) == tEnPassant);
}

std::string MoveToString(int move) {

    static const char prom_char[5] = "nbrq";
    std::string move_str = SquareName(GetFromSquare(move))
                         + SquareName(GetToSquare(move));

    if (IsMovePromotion(move)) {
        move_str += prom_char[(move >> 12) & 3];
    }

    return move_str;
}

int StringToMove(Position* pos, const std::string& moveString) {

    Square from, to;
    int type;
    std::string move_str;
    move_str = moveString;

    from = MakeSquare(move_str[0] - 'a', move_str[1] - '1');
    to = MakeSquare(move_str[2] - 'a', move_str[3] - '1');
    type = tNormal;

    if (pos->PieceTypeOnSq(from) == King && std::abs(to - from) == 2) {
        type = tCastle;
    }
    else if (pos->PieceTypeOnSq(from) == Pawn) {
        if (to == pos->EnPassantSq()) {
            type = tEnPassant;
        }
        else if (std::abs(to - from) == 16) {
            type = tPawnjump;
        }
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

