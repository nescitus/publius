#include "color.h"
#include "square.h"
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

	moves[ind] = (flag << 12) | (toSquare << 6) | fromSquare;
	ind++;
}

void MoveList::Clear() { 
    ind = 0; get = 0; 
}

int MoveList::GetInd() {
    return ind;
}

int MoveList::GetMove() { 

    int mv;
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
    mv = moves[loc];
    if (loc > get) SwapMoves(loc, get);
    get++;
    return mv; 
};

bool MoveList::HasMore() { 
	return (get < ind); 
};

void MoveList::ScoreMoves(Position * pos, int ply, int ttMove) {

	Square fromSquare, toSquare;
	int mType, mover, prey;

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
				mover = pos->PieceTypeOnSq(fromSquare);
				prey = pos->PieceTypeOnSq(toSquare);

                // capture
                if (prey != noPieceType) {

                    // this condition is just a placeholder for the proper
                    // bad capture detection / SEE
                    if (prey == Pawn && 
                        mover != Pawn &&
                        pos->IsPawnDefending(~pos->GetSideToMove(), toSquare ))
                        values[i] = 0 + 100 + 10 * prey - mover;
                    else
                        values[i] = IntLimit / 2 + 100 + 10 * prey - mover;
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
