// Publius - Didactic public domain bitboard chess engine by Pawel Koziol

#include <stdio.h>
#include "types.h"
#include "limits.h"
#include "publius.h"
#include "timer.h"
#include "history.h"
#include "pv.h"
#include "search.h"

int rootDepth;

void Think(Position* pos) {

    Timer.isStopping = false;
    History.Clear();
    nodeCount = 0;
    Timer.SetStartTime();
    Iterate(pos);
}

void Iterate(Position* pos) {

    int val = 0, curVal = 0;
    Bitboard nps = 0;

    Pv.Clear();

    for (rootDepth = 1; rootDepth <= Timer.GetData(maxDepth); rootDepth++) {
        
        int elapsed = Timer.Elapsed();
        if (elapsed)
            nps = nodeCount * 1000 / elapsed;

        PrintRootInfo(elapsed, nps);

        curVal = Widen(pos, rootDepth, curVal);

        // stop searching
        if (Timer.isStopping || Timer.ShouldFinishIteration())
            break;

        val = curVal;

        // for go infinite
        if (rootDepth == 64 && Timer.IsInfiniteMode() )
            Timer.waitingForStop = true;
    }
}

void PrintRootInfo(int elapsed, int nps) {

        std::cout << "info depth " << rootDepth
        << " time " << elapsed
        << " nodes " << nodeCount
        << " nps " << nps << std::endl;
}

int Widen(Position* pos, int depth, int lastScore) {

    int currentDepthScore = lastScore, alpha, beta;

    // apply aspiration window on sufficient depth
    // and if checkmate is not expected

    if (depth > 6 && lastScore < EvalLimit) {

        // progressively widen aspiration window

        for (int margin = 10; margin < 500; margin *= 2) {
            alpha = lastScore - margin;
            beta = lastScore + margin;
            currentDepthScore = Search(pos, 0, alpha, beta, depth, false, false);

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

    // full window search
    return Search(pos, 0, -Infinity, Infinity, depth, false, false);  
}
