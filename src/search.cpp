#include <stdio.h>
#include <string.h>
#include "math.h"
#include "color.h"
#include "square.h"
#include "publius.h"
#include "timer.h"
#include "history.h"
#include "trans.h"
#include "move.h"
#include "search.h"

// stack to hold information necessary to undo moves
UndoStack undoStack[stackSize];

// array keeping principal variation, used also to retrieve best move and ponder move
int pvLine[PlyLimit + 2][PlyLimit + 2];
int pvSize[PlyLimit + 2];

int lmr[2][64][64];

Bitboard nodeCount;

void InitLmr() {

    // Set depth of late move reduction

    for (int depth = 0; depth < 64; depth++)
        for (int moves = 0; moves < 64; moves++) {

            int r = log(depth) * log(moves) / 2;
            if (depth == 0 || moves == 0) r = 0;

            lmr[0][depth][moves] = r + 1; // zero window node
            lmr[1][depth][moves] = r;     // principal variation node
        }
}

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
    pvSize[ply] = ply;

    // Quick exit on a timeout

    if (State.isStopping) {
        return 0;
    }

    // Quick exit on on a statically detected draw, unless we are at root

    if (pos->IsDraw() && !isRoot) {
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

    if (!wasNull
    && !isInCheck 
    && !isPv
    &&  pos->IsNullMoveOk()) {

        eval = Evaluate(pos, &e);

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

    // Init moves and variables before entering main loop

    bestScore = -Infinity;
    list.Clear();
    FillCompleteList(pos, &list);
    moveListLength = list.GetInd();

    // Calculate moves' scores to sort them

    if (isRoot) 
        list.ScoreMoves(pos, ply, pvLine[0][0]);
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

            // Late move reduction (LMR)
            
            if (depth > 1 && 
                quietMovesTried > 3 && 
                moveType == moveQuiet && 
               !isInCheck && 
               !pos->IsInCheck())
            {   
                reduction = lmr[isPv]
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
                    RefreshPv(ply, move);
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
                    RefreshPv(ply, move);
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
        return moveHash;  

    if (IsMoveNoisy(pos, move))
        return moveNoisy;
   
    return moveQuiet;
}

void DisplayPv(int score) {

    std::string scoreType;
    Bitboard nps = 0;
    int elapsed = Timer.Elapsed();
    if (elapsed) nps = nodeCount * 1000 / elapsed;

    scoreType = "mate";
    if (score < -EvalLimit)     
        score = (-MateScore - score) / 2;
    else if (score > EvalLimit) 
        score = (MateScore - score + 1) / 2;
    else scoreType = "cp";

    std::cout << "info depth " << rootDepth
              << " time " << elapsed
              << " nodes " << nodeCount
              << " nps " << nps
              << " score "
              << scoreType << " " << score << " pv";

    for (int j = 0; j < pvSize[0]; ++j) {
        std::cout << " " << MoveToString(pvLine[0][j]);
    }

    std::cout << std::endl;

}

void TryInterrupting(void) 
{
    char command[80];

    if (nodeCount & 4095 || rootDepth == 1) {
        return;
    }

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

int Timeout() 
{
#ifdef USE_TUNING
    return false;
#endif
    return (!State.isPondering && 
            !Timer.IsInfiniteMode() && 
            Timer.TimeHasElapsed());
}
