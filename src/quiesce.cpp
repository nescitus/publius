// Publius - Didactic public domain bitboard chess engine 
// by Pawel Koziol

#include "types.h"
#include "limits.h"
#include "position.h"
#include "movelist.h"
#include "move.h"
#include "pv.h"
#include "evaldata.h"
#include "api.h"
#include "search.h"
#include "trans.h"
#include "timer.h"
#include "history.h"
#include "badcapture.h"
#include "movepicker.h"

int Quiesce(Position* pos, int ply, int qdepth, int alpha, int beta) {

    int bestScore, hashFlag, score;
    Move move, bestMove, ttMove;
    EvalData e;
    UndoData undo;
    MovePicker movePicker;
    bool saveInTT = true;

    // Init
    bestMove = 0;
    ttMove = 0;

    // Are we in a pv-node? See search.cpp for comments
    const bool isPv = (beta > alpha + 1);

    // Statistics
    Timer.nodeCount++;

    // Check for timeout
    TryInterrupting();

    // Exit to unwind search if it has timed out
    if (Timer.isStopping)
        return 0;

    // Retrieve score from transposition table
    // (in zero window nodes or when we get exact score)
    if (TT.Retrieve(pos->boardHash, &ttMove, &score, &hashFlag, alpha, beta, 0, ply)) {

        if (!isPv || (score > alpha && score < beta))
            return score;
    }

    Pv.size[ply] = ply;

    // Draw detection (and checking for timeout
    // in case too many draws detected in succession
    // mess with controlling the time)
    if (pos->IsDraw()) {
        Timer.TryStoppingByTimeout();
        return ScoreDraw;
    }

    // Safeguarding against ply limit overflow
    if (ply >= PlyLimit - 1)
        return Evaluate(pos, &e);

    // Are we in check? Then we must flee
    const bool isInCheck = pos->IsInCheck();

    // Get a stand-pat score and adjust bounds
    // (exiting if eval exceeds beta, but starting
    // with minus infinity when in check)
    bestScore = isInCheck ? -Infinity : Evaluate(pos, &e);

    // Static score cutoff
    if (bestScore >= beta)
        return bestScore;

    // Guaranteed score if we don't find anything better
    if (bestScore > alpha)
        alpha = bestScore;

    // Decide what subset of moves we want to generate.
    // In the first two plies of our quiescence search
    // we generate checking moves, later we restrict
    // ourselves to (good) captures, unless we are in
    // check - then we generate evasions.

    Mode movegenMode;

    if (isInCheck)
        movegenMode = modeAll;
    else if (qdepth <= 1) 
        movegenMode = modeChecks;
    else {
        movegenMode = modeCaptures;
        saveInTT = false;
    }

    movePicker.Init(ttMove, History.GetKiller1(ply), History.GetKiller2(ply) );

    while ((move = movePicker.NextMove(pos, ply, movegenMode)) != 0) {

        // Make move, unless illegal
        pos->DoMove(move, &undo);
        if (pos->IsOwnKingInCheck()) {
            pos->UndoMove(move, &undo);
                continue;
        }

        // Recursion
        score = -Quiesce(pos, ply + 1, qdepth + 1, -beta, -alpha);

        // Unmake move
        pos->UndoMove(move, &undo);

        // Exit in case of a timeout / stop command
        if (Timer.isStopping)
            return 0;

        // Beta cutoff
        if (score >= beta) {
            if (saveInTT)
                TT.Store(pos->boardHash, move, score, lowerBound, 0, ply);
            return score;
        }

        // Adjust alpha and score
        if (score > bestScore) {
            bestScore = score;
            if (score > alpha) {
                bestMove = move;
                alpha = score;
                Pv.Update(ply, move);
            }
        }
    }
    
    // Return correct checkmate/stalemate score
    if (bestScore == -Infinity)
        return pos->IsInCheck() ? -MateScore + ply : 0;

    // Save result in the transpositon table 
    if (saveInTT) {
        if (bestMove)
            TT.Store(pos->boardHash, bestMove, bestScore, exactEntry, 0, ply);
        else
            TT.Store(pos->boardHash, 0, bestScore, upperBound, 0, ply);
    }

    return bestScore;
}