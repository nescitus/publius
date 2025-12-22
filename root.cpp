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

void Think(Position* pos) {

    SearchContext sc;
    ClearSearchContext(sc);
    Pv.Clear();
    History.Clear();
    TT.Age();
    Timer.Start();
    Iterate(pos, &sc);

    // In ultra-rare cases where we don't get a move
    // because  time control is too short or we  got
    // a stop command, a move from the transposition
    // table is returned.
    if (Pv.GetBestMove() == 0) {
        Move move; int unused;
        TT.Retrieve(pos->boardHash, &move, &unused, &unused, -Infinity, Infinity, 0, 0);
        if (IsPseudoLegal(pos, move))
            Pv.EmergencyOverwrite(move);
    }
}

void Iterate(Position* pos, SearchContext* sc) {

    int val = 0, curVal = 0;

    for (Timer.rootDepth = 1; Timer.rootDepth <= Timer.GetData(maxDepth); Timer.rootDepth++) {

        Timer.RefreshStats();
        PrintRootInfo(); // uses timer stats

        // Stop searching - soft time limit reached
        if (Timer.ShouldNotStartIteration() || Timer.isStopping)
            break;

        curVal = Widen(pos, sc, Timer.rootDepth, curVal);

        // Stop searching when we are sure of a checkmate score
        // (the engine is given some depth to confirm that it
        //  cannot find a shorter checkmate)
        if (curVal > EvalLimit || curVal < -EvalLimit) {
            int expectedMateDepth = (MateScore - std::abs(curVal) + 1) + 1;
            if (Timer.rootDepth >= expectedMateDepth * 3 / 2)
                break;
        }

        val = curVal;

        // For go infinite, where we have to wait
        // for stop command before emitting a move
        if (Timer.rootDepth == 64 && Timer.IsInfiniteMode() )
            Timer.waitingForStop = true;
    }
}

void PrintRootInfo() {

        std::cout << "info depth " << Timer.rootDepth
        << " time " << Timer.timeUsed
        << " nodes " << Timer.nodeCount
        << " nps " << Timer.nps << std::endl;
}

int Widen(Position* pos, SearchContext* sc, int depth, int lastScore) {

    int currentDepthScore = lastScore, alpha, beta;

    // apply aspiration window on sufficient depth
    // and if checkmate is not expected

    if (depth > 6 && lastScore < EvalLimit) {

        // progressively widen aspiration window

        for (int margin = 10; margin < 500; margin *= 2) {
            alpha = lastScore - margin;
            beta = lastScore + margin;
            currentDepthScore = Search(pos, sc, 0, alpha, beta, depth, false, false);

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
        return Search(pos, sc, 0, -Infinity, Infinity, depth, false, false);  
}
