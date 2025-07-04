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
    allCaptureList.Clear();
    stage = stageTT;
}

Move MovePicker::NextMove(Position* pos, int ply) {

    Move move;

    while (true) {
        switch (stage) {
            case stageTT:
            {
                stage = stageGenCapt;
                if (IsPseudoLegal(pos, moveFromTT))
                    return moveFromTT;
                break;
            }

           case stageGenCapt:
            {
                FillNoisyList(pos, &allCaptureList);
                allCaptLength = allCaptureList.GetInd();
                allCaptCnt = 0;
                goodCaptureList.Clear();
                badCaptureList.Clear();

                while (allCaptCnt < allCaptLength) {
                    move = allCaptureList.GetMove();
                    if (IsBadCapture(pos, move))
                        badCaptureList.AddMove(move);
                    else
                        goodCaptureList.AddMove(move);
                    allCaptCnt++;
                }

                goodCaptureList.ScoreMoves(pos, ply, moveFromTT);
                goodCaptureLength = goodCaptureList.GetInd();
                goodCaptureCnt = 0;
                stage = stageReturnGoodCapt;
                break;
            }

            case stageReturnGoodCapt:
                {
                    while (goodCaptureCnt < goodCaptureLength) {
                        move = goodCaptureList.GetMove();
                        goodCaptureCnt++;
                        if (move == moveFromTT)
                            continue;  // Avoid returning moveFromTT again
                        return move;
                    }
                    stage = stageGenQuiet;
                    break;
                }

            case stageGenQuiet:
            {
                quietList.Clear();
                FillQuietList(pos, &quietList);
                quietList.ScoreQuiet(pos, ply, moveFromTT);
                quietLength = quietList.GetInd();
                quietCnt = 0;
                stage = stageReturnQuiet;
                break;
            }

            case stageReturnQuiet:
            {
                while (quietCnt < quietLength) {
                    move = quietList.GetMove();
                    quietCnt++;
                    if (move == moveFromTT)
                        continue;  // Avoid returning moveFromTT again
                    return move;
                }

                stage = stageReturnBad;
                badCaptureList.ScoreMoves(pos, ply, moveFromTT);
                badCaptureLength = badCaptureList.GetInd();
                badCaptureCnt = 0;
                break;
            }

            case stageReturnBad:
            {
                while (badCaptureCnt < badCaptureLength) {
                    move = badCaptureList.GetMove();
                    badCaptureCnt++;
                    if (move == moveFromTT)
                        continue;
                    return move;
                }
                return 0;
            }
        }
    }
}

// Bench at depth 15 took 23250 milliseconds, searching 32951946 nodes at 1417288 nodes per second.