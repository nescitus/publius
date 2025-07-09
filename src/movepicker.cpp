#include "types.h"
#include "publius.h"
#include "legality.h"
#include "gen.h"
#include "movepicker.h"

void MovePicker::Init(Move ttMove) {
    
    moveFromTT = ttMove;
    stage = stageTT;
}

Move MovePicker::NextMove(Position* pos, int ply, Mode mode) {

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
                    IsBadCapture(pos, move) ? badNoisyList.AddMove(move)
                                            : goodNoisyList.AddMove(move);
                }
                stage = stagePrepareGood;
                break;
            }

            case stagePrepareGood:
            {
                goodNoisyList.ScoreNoisy(pos);
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

                    // "modeCaptures" is used in the late stage 
                    // of quiescence search. In this case we try
                    // only good or equal captures, not bothering
                    // with the later stages of move generation.

                    stage = (mode == modeCaptures) ? stageEnd 
                                                   : stageGenQuiet;
                    break;
                }

            case stageGenQuiet:
            {
                quietList.Clear();
                (mode == modeChecks) ? FillCheckList(pos, &quietList) 
                                     : FillQuietList(pos, &quietList);
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

                stage = (mode == modeChecks) ? stageEnd
                                             : stagePrepareBad;
                break;
            }

            case stagePrepareBad:
            {
                stage = stageReturnBad;
                badNoisyList.ScoreNoisy(pos);
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
                stage = stageEnd;
                break;
            }

            case stageEnd: 
            {
                return 0;
            }
        }
    }
}