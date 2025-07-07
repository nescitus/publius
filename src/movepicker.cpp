#include "types.h"
#include "limits.h"
#include "publius.h"
#include "legality.h"
#include "movepicker.h"

void MovePicker::InitAllMoves(Move ttMove) {
    
    moveFromTT = ttMove;
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
                allNoisyList.Clear();
                FillNoisyList(pos, &allNoisyList);
                goodNoisyList.Clear();
                badNoisyList.Clear();

                // split noisy moves into "good"
                // (good or equal captures) and "bad"
                // (presumably losing material)
                while (true) {
                    move = allNoisyList.GetNextRawMove();
                    if (move == 0) 
                        break;
                    if (IsBadCapture(pos, move))
                        badNoisyList.AddMove(move);
                    else
                        goodNoisyList.AddMove(move);
                }
                stage = stagePrepareGood;
                break;
            }

            case stagePrepareGood:
            {
                goodNoisyList.ScoreNoisy(pos, ply, moveFromTT);
                goodNoisyLength = goodNoisyList.GetInd();
                goodNoisyCnt = 0;
                stage = stageReturnGoodCapt;
                break;
            }

            case stageReturnGoodCapt:
                {
                    while (goodNoisyCnt < goodNoisyLength) {
                        move = goodNoisyList.GetMove();
                        goodNoisyCnt++;
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

                stage = stagePrepareBad;
                break;
            }

            case stagePrepareBad:
            {
                stage = stageReturnBad;
                badNoisyList.ScoreNoisy(pos, ply, moveFromTT);
                badNoisyLength = badNoisyList.GetInd();
                badNoisyCnt = 0;
                break;
            }

            case stageReturnBad:
            {
                while (badNoisyCnt < badNoisyLength) {
                    move = badNoisyList.GetMove();
                    badNoisyCnt++;
                    if (move == moveFromTT)
                        continue;
                    return move;
                }
                return 0;
            }
        }
    }
}