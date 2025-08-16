// Publius - Didactic public domain bitboard chess engine 
// by Pawel Koziol

#include "types.h"
#include "position.h"
#include "movelist.h"
#include "legality.h"
#include "move.h"
#include "gen.h"
#include "badcapture.h"
#include "movepicker.h"

void MovePicker::Init(Move ttMove, Move firstKiller, Move secondKiller) {
    
    moveFromTT = ttMove;
    killer1 = firstKiller;
    killer2 = secondKiller;
    stage = stageTT;
    currentMoveStage = stageTT;
}

Move MovePicker::NextMove(Position* pos, int ply, Mode mode) {

    Move move;

    while (true) {
        switch (stage) {
            case stageTT:
            {
                stage = stageGenCapt;
                if (moveFromTT && IsPseudoLegal(pos, moveFromTT))
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
                goodNoisyLength = goodNoisyList.GetLength();
                goodNoisyCnt = 0;
                stage = stageReturnGoodCapt;
                currentMoveStage = stageReturnGoodCapt;
                break;
            }

            case stageReturnGoodCapt:
                {
                    while (goodNoisyCnt < goodNoisyLength) {
                        move = goodNoisyList.GetBestMove();
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
                                                   : stageFirstKiller;
                    break;
                }

            case stageFirstKiller:
            {
                stage = stageSecondKiller;
                if (IsAcceptableKiller(pos, killer1)) {
                    currentMoveStage = stageFirstKiller;
                    return killer1;
                }
            }

            case stageSecondKiller:
            {
                stage = stageGenQuiet;
                if (IsAcceptableKiller(pos, killer2)) {
                    currentMoveStage = stageFirstKiller;
                    return killer2;
                }
            }

            case stageGenQuiet:
            {
                quietList.Clear();
                (mode == modeChecks) ? FillCheckList(pos, &quietList) 
                                     : FillQuietList(pos, &quietList);

                quietList.ScoreQuiet(pos);
                quietLength = quietList.GetLength();
                quietCnt = 0;
                stage = stageReturnQuiet;
                currentMoveStage = stageReturnQuiet;
                break;
            }

            case stageReturnQuiet:
            {
                while (quietCnt < quietLength) {
                    move = quietList.GetBestMove();
                    quietCnt++;
                    
                    // Avoid returning moves tried at the earlier stages
                    if (move == moveFromTT || move == killer1 || move == killer2)
                        continue;

                    return move;
                }

                stage = (mode == modeChecks) ? stageEnd
                                             : stagePrepareBad;
                break;
            }

            case stagePrepareBad:
            {
                stage = stageReturnBad;
                currentMoveStage = stageReturnBad;
                badNoisyList.ScoreNoisy(pos);
                badNoisyLength = badNoisyList.GetLength();
                badNoisyCnt = 0;
                break;
            }

            case stageReturnBad:
            {
                while (badNoisyCnt < badNoisyLength) {
                    move = badNoisyList.GetBestMove();
                    badNoisyCnt++;
                    if (move == moveFromTT)
                        continue;
                    return move;
                }
                stage = stageEnd;
                break;
            }

            case stageEnd: 
                return 0;      
        }
    }
}

bool MovePicker::IsAcceptableKiller(Position* pos, Move killer) {
    return (killer && !IsMoveNoisy(pos, killer) && IsPseudoLegal(pos, killer));
}