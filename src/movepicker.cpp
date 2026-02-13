// Publius - Didactic public domain bitboard chess engine 
// by Pawel Koziol

// MovePicker class handles staged move generation.
// Depending on mode setting, it can generate  all
// the moves, just captures, or captures and checks.

// In the quiescence search nodes where checks 
// are generated, algorithm tries to be clever, 
// and throws quiet moves from the transposition
// table and killer moves in the mix (switching 
// this feature off failed a SPRT test).

#include "types.h"
#include "position.h"
#include "movelist.h"
#include "move.h"
#include "legality.h"
#include "gen.h"
#include "badcapture.h"
#include "movepicker.h"

//Initialize with the move ordering data
void MovePicker::Init(const Mode mode, const Move ttMove, const Move firstKiller, const Move secondKiller, const Move refuted) {
    
    moveFromTT = ttMove;
    killer1 = firstKiller;
    killer2 = secondKiller;
    stage = stageTT;
    currentMoveStage = stageTT;
    movegenMode = mode;
    refutedMove = refuted;
}

// Emits the next move
Move MovePicker::NextMove(Position* pos) {

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
                PrepareNoisyList(pos, goodNoisyList, goodNoisyLength, goodNoisyCnt);
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

                // "modeCaptures"  means we are in the  late  stage 
                // of quiescence search. We try only good or  equal 
                // captures here, skipping the later stages of move 
                // generation.

                stage = (movegenMode == modeCaptures) 
                      ? stageEnd 
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
                break;
            }

            case stageSecondKiller:
            {
                stage = stageGenQuiet;
                if (IsAcceptableKiller(pos, killer2)) {
                    currentMoveStage = stageSecondKiller;
                    return killer2;
                }
                break;
            }

            case stageGenQuiet:
            {
                quietList.Clear();
                (movegenMode == modeChecks) ? FillCheckList(pos, &quietList) 
                                            : FillQuietList(pos, &quietList);

                quietList.ScoreQuiet(pos, refutedMove);
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

                stage = (movegenMode == modeChecks) 
                      ? stageEnd
                      : stagePrepareBad;
                break;
            }

            case stagePrepareBad:
            {
                PrepareNoisyList(pos, badNoisyList, badNoisyLength, badNoisyCnt);
                stage = stageReturnBad;
                currentMoveStage = stageReturnBad;
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

// Scores a noisy move list and initializes counters.
// Used for both goodNoisyList and badNoisyList.
void MovePicker::PrepareNoisyList(Position* pos, MoveList& list, int& length, int& cnt) {

    list.ScoreNoisy(pos);
    length = list.GetLength();
    cnt = 0;
}

// Can we accept a killer move? Is it a quiet move, 
// legal and untried so far?
bool MovePicker::IsAcceptableKiller(Position* pos, const Move killer) {
    
    return (killer && 
            killer != moveFromTT && 
           !IsMoveNoisy(pos, killer) && 
            IsPseudoLegal(pos, killer));
}