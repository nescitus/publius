#include "publius.h"

int GetTypeOfMove(int move) {
    return move >> 12;
}

Square GetFromSquare(int move) {
    return (Square)(move & 63);
}

Square GetToSquare(int move) {
    return (Square)((move >> 6) & 63);
}

int GetPromotedPiece(int move) {
	return (move >> 12) - 3;
}

int CreateMove(Square fromSquare, Square toSquare, int flag) {
    return (flag << 12) | (toSquare << 6) | fromSquare;
}

bool IsMovePromotion(int move) {
	return move & 0x4000;
}

bool IsMoveNoisy(Position *pos, int move) {

    return (pos->GetPiece(GetToSquare(move)) != noPiece
        || IsMovePromotion(move)
        || GetTypeOfMove(move) == tEnPassant);
}

std::string MoveToString(int move) {

    static const char prom_char[5] = "nbrq";
    std::string move_str;

    move_str += FileOf(GetFromSquare(move)) + 'a';
    move_str += RankOf(GetFromSquare(move)) + '1';
    move_str += FileOf(GetToSquare(move)) + 'a';
    move_str += RankOf(GetToSquare(move)) + '1';

    if (IsMovePromotion(move)) {
        move_str += prom_char[(move >> 12) & 3];
    }

    return move_str;
}
