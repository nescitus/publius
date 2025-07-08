#include "types.h"
#include "limits.h"
#include "publius.h"
#include "history.h"
#include "move.h"
#include "piece.h"

void MoveList::SwapMoves(const int i, const int j) {

    Move tmpMove = moves[i];
    int tmpVal = values[i];
    moves[i] = moves[j];
    values[i] = values[j];
    moves[j] = tmpMove;
    values[j] = tmpVal;
}

void MoveList::AddMove(Move move) {

    moves[ind] = move;
    values[ind] = 0;
    ind++;
}

void MoveList::AddMove(Square fromSquare, Square toSquare, int flag) {

    moves[ind] = CreateMove(fromSquare, toSquare, flag);
    values[ind] = 0;
    ind++;
}

void MoveList::Clear() { 
    ind = 0; 
    get = 0; 
}

int MoveList::GetInd() {
    return ind;
}

Move MoveList::GetNextRawMove() {
    if (get < ind)
        return moves[get++];
    return 0; // 0 means "no more moves"
}

Move MoveList::GetMove() { 

    Move move;
    int min = -4 * HistLimit;
    int score = min;
    int loc = 0;

    for (int i = get; i < ind; i++) {
       if (values[i] > score) {
          score = values[i];
          loc = i;
       }
    }

    values[loc] = min;
    move = moves[loc];
    if (loc > get) 
        SwapMoves(loc, get);
    get++;
    return move; 
};

// Functions below rely on being called in correct places
// of a staged move generator

void MoveList::ScoreNoisy(Position* pos) {

    for (int i = 0; i < ind; i++) {

        values[i] = 5; // default, shouldn't be used

        if (pos->IsOccupied(GetToSquare(moves[i])))
            values[i] = 6 * pos->PieceTypeOnSq(GetToSquare(moves[i]))
                      + 5 - pos->PieceTypeOnSq(GetFromSquare(moves[i]));

        if (IsMovePromotion(moves[i]))
            values[i] = GetPromotedPiece(moves[i]) - 5;
    }
}

void MoveList::ScoreQuiet(Position* pos, const int ply, const Move ttMove) {

    for (int i = 0; i < ind; i++) {

        // first killer move
        if (moves[i] == History.GetKiller1(ply))
            values[i] = Killer1Value;
        // second killer move
        else if (moves[i] == History.GetKiller2(ply))
            values[i] = Killer2Value;
        // normal move
        else
            values[i] = History.GetScore(pos, moves[i]);    
    }
}