#include <stdio.h>
#include <string.h>
#include "color.h"
#include "square.h"
#include "limits.h"
#include "publius.h"
#include "timer.h"
#include "history.h"
#include "trans.h"
#include "move.h"
#include "lmr.h"
#include "pv.h"
#include "evaldata.h"
#include "eval.h"
#include "search.h"

// stack to hold information necessary to undo moves
UndoStack undoStack[stackSize];

Bitboard nodeCount;

int Search(Position *pos, int ply, int alpha, int beta, int depth, bool wasNull) {

    int bestScore, newDepth, eval, moveListLength;
    int isInCheck, hashFlag, reduction, score, moveType;
    int move, ttMove, bestMove, movesTried, quietMovesTried;
    bool isRoot, isPv;
    EvalData e;
    MoveList list;

    // Init

    move = 0;
    ttMove = 0;
    bestMove = 0;
    movesTried = 0;
    quietMovesTried = 0;
    isRoot = !ply;
    isPv = (beta > alpha + 1);

    // Quiescence search entry point

    if (depth <= 0) {
        return Quiesce(pos, ply, alpha, beta);
    }

    // Periodically check for timeout, ponderhit or stop command

    nodeCount++;
    TryInterrupting();
    Pv.size[ply] = ply;

    // Quick exit on a timeout

    if (State.isStopping) {
        return 0;
    }

    // Quick exit on on a statically detected draw, unless we are at root

    if (pos->IsDraw() && !isRoot) {
        // Too many early exits in a row 
        // might cause a timeout, so we safeguard
        if (Timeout())
            State.isStopping = true;
        return 0;
    }

    // Retrieving data from transposition table

    if (TT.Retrieve(pos->boardHash, &ttMove, &score, &hashFlag, alpha, beta, depth, ply)) {

        if (!isPv || (score > alpha && score < beta)) {
            return score;
        }
    }

    // Safeguard against exceeding ply limit

    if (ply >= PlyLimit - 1) {
        return Evaluate(pos, &e);
    }

    // Are we in check? 

    isInCheck = pos->IsInCheck();

    // Node-level pruning

    if (!wasNull &&
       !isInCheck &&
       !isPv &&
        pos->CanTryNullMove()) 
    {

        eval = Evaluate(pos, &e);

        // Static null move / RFP

        if (depth <= 6) {
            score = eval - 75 * depth;
            if (score > beta)
                return score;
        }

        // Null move

        if (eval > beta &&  depth > 1) {
            reduction = 3 + depth / 6;
            pos->DoNull(ply);
            score = -Search(pos, ply + 1, -beta, -beta + 1, depth - reduction, true);
            pos->UndoNull(ply);

            if (State.isStopping) {
                return 0;
            }

            if (score >= beta) {
                return score;
            }
        }   // end of null move code
    }

    // Set futility pruning flag

    bool canDoFutility = false;

    if (depth <= 6 &&
        !isInCheck &&
        !isPv &&
        Evaluate(pos, &e) + 75 * depth < beta) {
        canDoFutility = true;
    }

    // Init moves and variables before entering main loop

    bestScore = -Infinity;
    list.Clear();
    FillCompleteList(pos, &list);
    moveListLength = list.GetInd();

    // Calculate moves' scores to sort them

    if (isRoot) 
        list.ScoreMoves(pos, ply, Pv.line[0][0]);
    else      
        list.ScoreMoves(pos, ply, ttMove);

    // Check extension

    if (isInCheck) depth++;

    // Main loop

    if (moveListLength) {
        for (int i = 0; i < moveListLength; i++) {

            move = list.GetMove();
            moveType = GetMoveType(pos, move, ttMove);

            pos->DoMove(move, ply);

            if (pos->LeavesKingInCheck()) {
                pos->UndoMove(move, ply);
                continue;
            }

            // Update move statistics

            movesTried++;
            if (moveType == moveQuiet) {
                quietMovesTried++;
            }

            // Set new search depth

            newDepth = depth - 1;

            // Futility pruning

            if (canDoFutility &&
                movesTried > 1 &&
               !isPv &&
               !isInCheck &&
               !pos->IsInCheck() &&
                moveType == moveQuiet) 
            {
                pos->UndoMove(move, ply);
                continue;
            }

            // Late move pruning

            if (depth <= 3 &&
               !isPv && 
               !isInCheck &&
               !pos->IsInCheck() &&
                moveType == moveQuiet && 
                quietMovesTried > 4 * depth) 
            {
                pos->UndoMove(move, ply);
                continue;
            }

            // Late move reduction (LMR)
            
            if (depth > 1 && 
                quietMovesTried > 3 && 
                moveType == moveQuiet && 
               !isInCheck && 
               !pos->IsInCheck())
            {   
                reduction = Lmr.table[isPv]
                                     [std::min(depth,63)]
                                     [std::min(movesTried, 63)];
                if (reduction >= newDepth)
                    reduction = newDepth - 1;

                if (reduction > 0) {
                    score = -Search(pos, ply + 1, -alpha - 1, -alpha, newDepth - reduction, false);
                    
                    if (score <= alpha) {
                        pos->UndoMove(move, ply);
                        if (State.isStopping) return 0;
                        continue;
                    }
                }
            }

            // PVS

            if (bestScore == -Infinity)
                score = -Search(pos, ply + 1, -beta, -alpha, newDepth, false);
            else {
                score = -Search(pos, ply + 1, -alpha - 1, -alpha, newDepth, false);
                if (!State.isStopping && score > alpha)
                    score = -Search(pos, ply + 1, -beta, -alpha, newDepth, false);
            }

            pos->UndoMove(move, ply);
            if (State.isStopping) {
                return 0;
            }

            // Beta cutoff

            if (score >= beta) {
                History.Update(pos, move, depth, ply);
                TT.Store(pos->boardHash, move, score, upperBound, depth, ply);

                // If beta cutoff occurs at the root, 
                // change the best move

                if (isRoot) {
                    Pv.Refresh(ply, move);
                    DisplayPv(score);
                }

                return score;
            }

            // Updating score and alpha

            if (score > bestScore) {
                bestScore = score;
                if (score > alpha) {
                    alpha = score;
                    bestMove = move;
                    Pv.Refresh(ply, move);
                    if (isRoot) {
                        DisplayPv(score);
                    }
                }
            }
        }
    } // end of the main loop

    // Return correct checkmate/stalemate score

    if (bestScore == -Infinity) {
        return pos->IsInCheck() ? -MateScore + ply : 0;
    }

    // Save score in the transposition table

    if (bestMove) {
        TT.Store(pos->boardHash, bestMove, bestScore, exactEntry, depth, ply);
    } else {
        TT.Store(pos->boardHash, 0, bestScore, lowerBound, depth, ply);
    }

    return bestScore;
}

int GetMoveType(Position *pos, int move, int ttMove) {

    if (move == ttMove) 
        return moveHash;  // move from the hash table

    if (IsMoveNoisy(pos, move))
        return moveNoisy; // capture or promotion
   
    return moveQuiet;     // quiet move
}

void DisplayPv(int score) {

    std::string scoreType;
    Bitboard nps = 0;
    int elapsed = Timer.Elapsed();

    // calculate nodes per second
    if (elapsed) nps = nodeCount * 1000 / elapsed;

    // If we are outside of normal evaluation range,
    // then the engine either gives a checkmate
    // or is being mated. In this case, we translate
    // the score into distance to mate and set
    // approppriate score type ("mate" instead of
    // the usual centipawns)
    scoreType = "mate";
    if (score < -EvalLimit)     
        score = (-MateScore - score) / 2;
    else if (score > EvalLimit) 
        score = (MateScore - score + 1) / 2;
    else scoreType = "cp";

    // print statistics
    std::cout << "info depth " << rootDepth
              << " time " << elapsed
              << " nodes " << nodeCount
              << " nps " << nps
              << " score "
              << scoreType << " " << score << " pv";

    // print main line
    for (int j = 0; j < Pv.size[0]; ++j) {
        std::cout << " " << MoveToString(Pv.line[0][j]);
    }

    std::cout << std::endl;

}

void TryInterrupting(void) 
{
    char command[80];

    // We don't check for timeout in every node,
    // but only every so often, to improve speed
    if (nodeCount & 4095 || rootDepth == 1) {
        return;
    }

    // Search limited by the nodecount
    // (not entirely precise, see previous comment)
    if (Timer.GetData(maxNodes) != 0) {
        if (nodeCount >= Timer.GetData(maxNodes))
            State.isStopping = true;
    }

    if (InputAvailable()) {
 
        std::cin.getline(command, 4096);
        if (!strcmp(command, "stop"))
            State.isStopping = true;
        else if (!strcmp(command, "ponderhit"))
            State.isPondering = false;
        else if (!strcmp(command, "isready"))
            std::cout << "readyok" << std::endl;
    }

    if (Timeout()) {
        State.isStopping = true;
    }
}

int Timeout() {

    return (!State.isPondering && 
            !Timer.IsInfiniteMode() && 
            Timer.TimeHasElapsed());
}
