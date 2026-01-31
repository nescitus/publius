// Publius - Didactic public domain bitboard chess engine by Pawel Koziol

#pragma once

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

// Staged move generator goes through the following
// stages, from top to bottom:

enum {
    stageTT,              // return move from transposition table 
    stageGenCapt,         // generate noisy moves, split them to good and bad
    stagePrepareGood,     // score good noisies
    stageReturnGoodCapt,  // return good noisies
    stageFirstKiller,     // first killer move
    stageSecondKiller,    // second killer move
    stageGenQuiet,        // generate and score quiet moves
    stageReturnQuiet,     // return quiet moves
    stagePrepareBad,      // score bad noisies
    stageReturnBad,       // return bad noisies
    stageEnd };           // no more moves

// MovePicker can be run in a few different modes:

enum Mode { modeAll,        // main search or check evasion in qs
            modeChecks,     // check-aware quiescence search
            modeCaptures }; // basic (late) quiescence search

class MovePicker {
private:
    Mode movegenMode;
    int goodNoisyLength, badNoisyLength, quietLength;
    int goodNoisyCnt, badNoisyCnt, quietCnt;
    MoveList allNoisyList, goodNoisyList, badNoisyList, quietList;
    Move killer1;
    Move killer2;
    Move oldMove;
    bool IsAcceptableKiller(Position* pos, Move killer);
    bool IsOkForMode(Position* pos, Move move);  
public:
    Move moveFromTT;
    int stage;
    int currentMoveStage;
    void Init(Mode mode, Move ttMove, Move firstKiller, Move secondKiller, Move old);
    Move NextMove(Position* pos, int ply);
};