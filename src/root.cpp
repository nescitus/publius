// Publius - Didactic public domain bitboard chess engine 
// by Pawel Koziol

#include <iostream>
#include "types.h"
#include "limits.h"
#include "position.h"
#include "timer.h"
#include "history.h"
#include "pv.h"
#include "move.h"
#include "legality.h"
#include "search.h"
#include "trans.h"
#include "publius.h"

ExcludedMoves rootExclusions;

void Think(Position* pos) {

    SearchContext context;

    // Init
    ClearSearchContext(context);
    Pv.Clear();
    History.Clear();
    TT.Age();
    Timer.Start();

    // Search, increasing depth, until stopped.
    Iterate(pos, &context);

    // In ultra-rare cases we don't get a move because  the 
    // time  control is too short or we got a stop  command. 
    // A move from the transposition table is returned then.
    if (Pv.GetBestMove() == 0)
        OverwriteFromTT(pos);
}

// Replace the first move of pv with the move from the transposition table,
// if possible.
void OverwriteFromTT(Position *pos) 
{
    Move move;
    int unused; // TT.Retrieve() wants to set flags and we don't need them
    
    TT.Retrieve(pos->boardHash, &move, &unused, &unused, -Infinity, Infinity, 0, 0);
    
    if (IsPseudoLegal(pos, move))
        Pv.Overwrite(move);
}

// Search with inceasing depth
void Iterate(Position* pos, SearchContext* context) {

    int val = 0, curVal = 0;
    rootExclusions.Clear();

    for (Timer.rootDepth = 1; Timer.rootDepth <= Timer.GetData(maxDepth); Timer.rootDepth++) {

        // Do not start searching at a greater depth 
        // - soft time limit reached
        if (Timer.ShouldNotStartIteration() || Timer.isStopping)
            break;

        // Diplay stats when begining search to a new depth
        Timer.RefreshStats();
        if (multiPv == 1) PrintRootInfo(); // uses timer stats

        // Search
        if (multiPv == 1)
            curVal = Widen(pos, context, Timer.rootDepth, curVal);
        else
            curVal = MultiPv(pos, context, Timer.rootDepth);

        // Stop searching when we are sure of a checkmate score
        // (the engine is given some depth to confirm that it
        //  cannot find a shorter checkmate)
        if ((curVal > EvalLimit || curVal < -EvalLimit) && multiPv == 1) {
            int expectedMateDepth = (MateScore - std::abs(curVal) + 1) + 1;
            if (Timer.rootDepth >= expectedMateDepth * 3 / 2)
                break;
        }

        Pv.RememberBestLine(); // hack for correct upperbound display

        val = curVal;

        // For go infinite, where we have to wait
        // for stop command before emitting a move
        if (Timer.rootDepth == 64 && Timer.IsInfiniteMode() )
            Timer.waitingForStop = true;
    }
}

// Search to find n best moves
int MultiPv(Position* pos, SearchContext* context, int depth) {

    MultiPVLines lines(multiPv);
    lines.Clear();
    rootExclusions.Clear();

    // Fallback move from previous iteration (or TT later)
    Move fallbackMove = Pv.GetBestMove();

    // 1) First PV (no exclusions yet)
    int score = Search(pos, context, 0, -Infinity, Infinity, depth, false, false);

    if (!Timer.IsTimeout()) {
        Move move = Pv.GetBestMove();
        if (move) {
            rootExclusions.Add(move);
            lines.Add(score, move, Pv.GetOutputStringWithoutDepth(score, exactEntry));
        }
    }
    else {
        if (fallbackMove) Pv.Overwrite(fallbackMove);
        return score; // or fallback score; up to you
    }

    // 2) Remaining PVs
    for (int i = 1; i < multiPv; ++i) {

        Pv.Clear();

        score = Search(pos, context, 0, -Infinity, Infinity, depth, false, false);
        if (Timer.IsTimeout()) break;

        Move move = Pv.GetBestMove();
        if (!move) break;

        rootExclusions.Add(move);
        lines.Add(score, move, Pv.GetOutputStringWithoutDepth(score, exactEntry));
    }

    lines.DisplayAll(depth, multiPv);

    // Ensure correct move is played (best after sorting)
    Move bestMove = lines.GetBestMove();
    if (bestMove) 
        Pv.Overwrite(bestMove);
    else if (fallbackMove) 
        Pv.Overwrite(fallbackMove);

    return lines.GetBestScore(); // consistent with chosen best move
}

void PrintRootInfo() {

    std::cout << "info depth " << Timer.rootDepth
              << " time " << Timer.timeUsed
              << " nodes " << Timer.nodeCount
              << " nps " << Timer.nps << std::endl;
}

// Search with increasing aspiration window
int Widen(Position* pos, SearchContext* context, int depth, int lastScore) {

    int currentDepthScore = lastScore, alpha, beta;

    // Apply aspiration window if sufficient depth has been reached
    // and if checkmate is not expected

    if (depth > 6 && lastScore < EvalLimit) {

        // progressively widen aspiration window

        for (int margin = 10; margin < 500; margin *= 2) {
            alpha = lastScore - margin;
            beta = lastScore + margin;
            currentDepthScore = Search(pos, context, 0, alpha, beta, depth, false, false);

            // timeout
            if (Timer.isStopping)
                break;

            // we have finished within the window, break the loop
            if (currentDepthScore > alpha && currentDepthScore < beta)
                return currentDepthScore;

            // verify a checkmate by searching with infinite bounds
            if (currentDepthScore > EvalLimit)
                break;
        }
    }

    // full window search, unless we broke out of the loop due to timeout
    if (Timer.isStopping) 
        return lastScore;
    else
        return Search(pos, context, 0, -Infinity, Infinity, depth, false, false);  
}
