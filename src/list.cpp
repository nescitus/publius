#include "color.h"
#include "square.h"
#include "limits.h"
#include "publius.h"
#include "history.h"
#include "move.h"
#include "piece.h"

void MoveList::SwapMoves(const int i, const int j) {

    int tmpMove = moves[i];
    int tmpVal = values[i];
    moves[i] = moves[j];
    values[i] = values[j];
    moves[j] = tmpMove;
    values[j] = tmpVal;
}

void MoveList::AddMove(Square fromSquare, Square toSquare, int flag) {

    moves[ind] = CreateMove(fromSquare, toSquare, flag);
	ind++;
}

void MoveList::Clear() { 
    ind = 0; get = 0; 
}

int MoveList::GetInd() {
    return ind;
}

int MoveList::GetMove() { 

    int move;
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
    if (loc > get) SwapMoves(loc, get);
    get++;
    return move; 
};

void MoveList::ScoreMoves(Position *pos, 
                          const int ply, 
                          const int ttMove) {

	Square fromSquare, toSquare;
	int mType, hunter, prey;

	for (int i = 0; i < ind; i++) {
        // hash move
		if (moves[i] == ttMove) 
			values[i] = IntLimit;
		else {
			values[i] = 0; // default, shouldn't be used

			mType = GetTypeOfMove(moves[i]);

			if (mType == tNormal || mType == tPawnjump || mType == tCastle) {

				fromSquare = GetFromSquare(moves[i]);
				toSquare = GetToSquare(moves[i]);
				hunter = pos->PieceTypeOnSq(fromSquare);
				prey = pos->PieceTypeOnSq(toSquare);

                // capture
                if (prey != noPieceType) {

                    if (IsBadCapture(pos, moves[i]))
                        values[i] = 0 + 100 + 10 * prey - hunter;
                    else
                        values[i] = IntLimit / 2 + 100 + 10 * prey - hunter;
                }
                // quiet move
                else {
                    // first killer move
                    if (moves[i] == History.GetKiller1(ply))
                        values[i] = IntLimit / 2;
                    // second killer move
                    else if (moves[i] == History.GetKiller2(ply))
                        values[i] = IntLimit / 2 - 1;
                    // normal move
                    else
                        values[i] = History.GetScore(pos, moves[i]);
                }
			}

            // en passant capture
			if (mType == tEnPassant) 
                values[i] = IntLimit / 2 + 106;

            // promotions
			if (IsMovePromotion(moves[i])) {
				if (mType == tPromQ) values[i] = IntLimit / 2 + 110; // TODO: table will be shorter
				if (mType == tPromN) values[i] = IntLimit / 2 + 109;
				if (mType == tPromR) values[i] = IntLimit / 2 + 108;
				if (mType == tPromB) values[i] = IntLimit / 2 + 107;
			}
		}
	}
}