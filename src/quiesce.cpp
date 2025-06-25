#include "types.h"
#include "square.h"
#include "limits.h"
#include "publius.h"
#include "move.h"
#include "pv.h"
#include "evaldata.h"
#include "eval.h"
#include "search.h"
#include "trans.h"

int Quiesce(Position* pos, int ply, int qdepth, int alpha, int beta) {

    int bestScore, hashFlag, score;
    Move move, bestMove, ttMove;
    EvalData e;
    MoveList list;
    UndoData undo;

    // Init
    bestMove = 0;
    ttMove = 0;

    // Are we in a pv-node? See search.cpp for comments
    const bool isPv = (beta > alpha + 1);

    // Statistics
    nodeCount++;

    // Check for timeout
    TryInterrupting();

    // Exit to unwind search if it has timed out
    if (State.isStopping) {
        return 0;
    }

    // Retrieve score from transposition table
    // (in zero window nodes or when we get exact score)
    if (TT.Retrieve(pos->boardHash, &ttMove, &score, &hashFlag, alpha, beta, 0, ply)) {

        if (!isPv || (score > alpha && score < beta)) {
            return score;
        }
    }

    Pv.size[ply] = ply;

    // Draw detection
    if (pos->IsDraw()) {

        // Too many early exits in a row 
        // might cause a timeout, so we safeguard
        if (Timeout())
            State.isStopping = true;

        return ScoreDraw;
    }

    // Safeguarding against ply limit overflow
    if (ply >= PlyLimit - 1) {
        return Evaluate(pos, &e);
    }

    // Are we in check? Then we must flee
    const bool isInCheck = pos->IsInCheck();

    // Get a stand-pat score and adjust bounds
    // (exiting if eval exceeds beta
    // but starting at the lowest possible value
    // when in check)
    bestScore = isInCheck ? -Infinity : Evaluate(pos, &e);

    // Static score cutoff
    if (bestScore >= beta) {
        return bestScore;
    }

    // Guaranteed score if we don't find anything better
    if (bestScore > alpha) {
        alpha = bestScore;
    }

    // Generate and sort move list. We have three cases:
    // 1) when in check, we look for evasions
    // 2) when we have just started quiescence search,
    //    we generate captures and (some) checks
    // 3) otherwise it's just captures
    list.Clear();
    if (isInCheck)
        FillCompleteList(pos, &list); // case 1)
    else if (qdepth == 0) // <= 1 failed 2025-03-16
        FillChecksAndCaptures(pos, &list); // case 2)
    else
        FillNoisyList(pos, &list); // case 3)

    // Score moves to sort them well
    int length = list.GetInd();
    list.ScoreMoves(pos, ply, ttMove);

    // Main loop
    if (length) {

        for (int i = 0; i < length; i++) {

            move = list.GetMove();

            // Bad capture pruning. If the capture
            // is expected to lose material, we aren't
            // trying it in the quiescence search.
            // We don't do it while in check, because
            // all the moves defending against the check
            // need to be tried.
            if (!isInCheck && !pos->IsEmpty(GetToSquare(move))) {
                if (IsBadCapture(pos, move))
                    continue;
            }

            // Make move, unless illegal
            pos->DoMove(move, &undo);
            if (pos->LeavesKingInCheck()) {
                pos->UndoMove(move, &undo);
                continue;
            }

            // Recursion
            score = -Quiesce(pos, ply + 1, qdepth + 1, -beta, -alpha);

            // Unmake move
            pos->UndoMove(move, &undo);

            // Exit if needed
            if (State.isStopping) {
                return 0;
            }

            // Beta cutoff
            if (score >= beta) {
                TT.Store(pos->boardHash, move, score, upperBound, 0, ply);
                return score;
            }

            // Adjust alpha and score
            if (score > bestScore) {
                bestScore = score;
                if (score > alpha) {
                    bestMove = move;
                    alpha = score;
                    Pv.Refresh(ply, move);
                }
            }
        }
    }

    // Return correct checkmate/stalemate score
    if (bestScore == -Infinity) {
        return pos->IsInCheck() ? -MateScore + ply : 0;
    }

    // Save result in the transpositon table 
    // (depth 0 for quiescence search)
    if (bestMove) {
        TT.Store(pos->boardHash, bestMove, bestScore, exactEntry, 0, ply);
    } else {
        TT.Store(pos->boardHash, 0, bestScore, lowerBound, 0, ply);
    }

    return bestScore;
}