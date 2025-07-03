#include "types.h"
#include "limits.h"
#include "publius.h"
#include "legality.h"
#include "movepicker.h"

// MovePicker class is a framework for staged
// move generation. The idea is to delay generating
// all the moves as much as possible.

// Please note that even something as simple 
// as separating a stage that returns a move
// from the transposition table changes node counts.
// Colin Jenkins, author of Lozza, explained it
// as follows: "consider a position with 3 moves 
// (m1,2), (m2,2), (tt,100). in a non-staged context 
// the moves are served as {tt, m2, m1} because m1 
// is swapped with tt (assuming > condition). 
// in a staged move context tt is not there and 
// the moves are served as {tt}, {m1, m2}."

void MovePicker::InitAllMoves(Move ttMove) {
    
    moveFromTT = ttMove;
    list.Clear();
    stage = stageTT;
}

Move MovePicker::NextMove(Position* pos, int ply) {

    Move move;

    while (true) {
        switch (stage) {
        case stageTT:
            stage = stageGen;
            if (IsPseudoLegal(pos, moveFromTT))
                return moveFromTT;
            break;

        case stageGen:
            FillCompleteList(pos, &list);
            list.ScoreMoves(pos, ply, moveFromTT);
            listLength = list.GetInd();
            cnt = 0;
            stage = stageReturn;
            break;

        case stageReturn:
            while (cnt < listLength) {
                move = list.GetMove();
                cnt++;
                if (move == moveFromTT)
                    continue;  // Avoid returning moveFromTT again
                return move;
            }

            return 0;
        }
    }
}

// Bench at depth 15 took 25437 milliseconds, searching 34483645 nodes at 1355649 nodes per second.