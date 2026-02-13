// Publius - Didactic public domain bitboard chess engine by Pawel Koziol

#pragma once

// MovePicker  class  handles  staged  move  generation. The  idea  
// is  to avoid generating all the  moves  if  possible. This  way,  
// if we get a cutoff on a move from the  transposition table,  we  
// will skip the move generation entirely, and if a  good  capture 
// causes  a cutoff, we omit quiet move  generation.  MovegenStage
// enum shows the exact stages.

// You can think of MovePicker as a finite state machine that will
// eventually return all the required moves, but is lazy and  asks
// several times: "Do you have a cutoff so that I can stop?"

// Please note that even something as simple as separating a stage 
// that  returns a move from the transposition table changes  node 
// counts. Colin Jenkins, author of Lozza, explained it as follows: 
// "consider  a  position  with  3 moves  (m1,2), (m2,2), (tt,100). 
// In  a  non-staged context the moves are served  as {tt, m2, m1}
// because  m1  is  swapped with tt  (assuming > condition).  in a  
// staged gen context tt is not there and the moves are served  as 
// {tt}, {m1, m2}."

// Staged move generator goes through the following
// stages, from top to bottom:

enum MovegenStage {
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

// MovePicker can be run in a few different modes. Think of them as
// specific paths, skipping certain stages if they are not necessary
// in the current context:

enum Mode { modeAll,        // main search or check evasion in qs
            modeChecks,     // check-aware quiescence search
            modeCaptures }; // basic (late) quiescence search

class MovePicker {
private:
    Mode movegenMode;
    int goodNoisyLength, badNoisyLength, quietLength;
    int goodNoisyCnt, badNoisyCnt, quietCnt;
    MovegenStage stage;
    MoveList allNoisyList, goodNoisyList, badNoisyList, quietList;
    Move moveFromTT;
    Move killer1, killer2;
    Move refutedMove; // needed to sort quiet moves by refutation history

    void PrepareNoisyList(Position* pos, MoveList& list, int& length, int& cnt);
    bool IsAcceptableKiller(Position* pos, const Move killer);
public:
    MovegenStage currentMoveStage; // read in search.cpp
    void Init(const Mode mode, const Move ttMove, const Move firstKiller, const Move secondKiller, const Move refuted);
    Move NextMove(Position* pos);
};