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

bool MoveList::Contains(Move move) {

    for (int i = 0; i < ind; i++) {
        if (moves[i] == move)
            return true;
    }

    return false;
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

// Intended move ordering is:
// - move from transposition table
// - good and equal captures
// - killer moves
// - quiet moves
// - bad captures
//
// However, bad captures can mix
// with quiet moves that have very low
// history score. This is illogical, 
// but fixing it failed. The issue
// will be revisited when implementing
// staged move generation.

void MoveList::ScoreAllMoves(Position* pos, 
                          const int ply, 
                          const Move ttMove) {

    Square fromSquare, toSquare;
    int moveType, hunter, prey;

    for (int i = 0; i < ind; i++) {
        // hash move
        if (moves[i] == ttMove) 
            values[i] = IntLimit;
        else {
            values[i] = 0; // default, shouldn't be used

            moveType = GetTypeOfMove(moves[i]);

            if (moveType == tNormal || moveType == tPawnjump || moveType == tCastle) {

                fromSquare = GetFromSquare(moves[i]);
                toSquare = GetToSquare(moves[i]);
                hunter = pos->PieceTypeOnSq(fromSquare);
                prey = pos->PieceTypeOnSq(toSquare);

                // capture
                if (prey != noPieceType) {

                    // NOTE: current setup lead to mixing
                    // bad captures with moves having 
                    // very low history score.
                    if (IsBadCapture(pos, moves[i]))
                        values[i] = BadCaptureValue + 10 * prey - hunter;
                    else
                        values[i] = GoodCaptureValue + 10 * prey - hunter;
                }
                // quiet move
                else {
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

            // en passant capture
            if (moveType == tEnPassant) 
                values[i] = HighValue + 106;

            // promotions
            if (IsMovePromotion(moves[i])) {
                if (moveType == tPromQ) values[i] = QueenPromValue;
                if (moveType == tPromN) values[i] = KnightPromValue;
                if (moveType == tPromR) values[i] = RookPromValue;
                if (moveType == tPromB) values[i] = BishopPromValue;
            }
        }
    }
}

// Functions below rely on being called in correct places
// of a staged move generator

void MoveList::ScoreNoisy(Position* pos,
    const int ply,
    const Move ttMove) {

    Square fromSquare, toSquare;
    int moveType, hunter, prey;

    for (int i = 0; i < ind; i++) {

        values[i] = 0; // default, shouldn't be used

        moveType = GetTypeOfMove(moves[i]);

        if (moveType == tNormal) {

            fromSquare = GetFromSquare(moves[i]);
            toSquare = GetToSquare(moves[i]);
            hunter = pos->PieceTypeOnSq(fromSquare);
            prey = pos->PieceTypeOnSq(toSquare);

            // capture (guaranteed to be good)
            if (prey != noPieceType) {
               values[i] = GoodCaptureValue + 10 * prey - hunter;
            }
        }

        // en passant capture
        if (moveType == tEnPassant)
            values[i] = HighValue + 106;

        // promotions
        if (IsMovePromotion(moves[i])) {
            if (moveType == tPromQ) values[i] = QueenPromValue;
            if (moveType == tPromN) values[i] = KnightPromValue;
            if (moveType == tPromR) values[i] = RookPromValue;
            if (moveType == tPromB) values[i] = BishopPromValue;
        }    
    }
}

void MoveList::ScoreQuiet(Position* pos,
    const int ply,
    const Move ttMove) {

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