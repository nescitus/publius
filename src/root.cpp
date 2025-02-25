// Publius 1.0. Didactic bitboard chess engine by Pawel Koziol

#include <stdio.h>
#include "color.h"
#include "square.h"
#include "publius.h"
#include "timer.h"
#include "history.h"
#include "search.h"

int rootDepth;

void Think(Position *p) {

    State.isStopping = false;
    History.Clear();
    nodeCount = 0;
    Timer.SetStartTime();
    Iterate(p);
}

void Iterate(Position *p) {

    int val = 0, curVal = 0;
    Bitboard nps = 0;

    ClearPvLine();

    for (rootDepth = 1; rootDepth <= Timer.GetData(maxDepth); rootDepth++) {
        
        int elapsed = Timer.Elapsed();
        
        if (elapsed) {
            nps = nodeCount * 1000 / elapsed;
        }

        PrintRootInfo(elapsed, nps);

	    curVal = Widen(p, rootDepth, curVal);

        if (State.isStopping) {
            break;
        }

        val = curVal;
    }
}

void ClearPvLine() {

    for (int i = 0; i < 66; i++)
        for (int j = 0; j < 66; j++)
            pvLine[i][j] = 0;
}

void PrintRootInfo(int elapsed, int nps) {

        std::cout << "info depth " << rootDepth
        << " time " << elapsed
        << " nodes " << nodeCount
        << " nps " << nps
        << std::endl;
}

int Widen(Position *pos, int depth, int lastScore) {

    int currentDepthScore = lastScore, alpha, beta;

    // apply aspiration window on sufficient depth
    // and if checkmate is not expected

    if (depth > 6 && lastScore < EvalLimit) {

        // progressively widen aspiration window

        for (int margin = 10; margin < 500; margin *= 2) {
            alpha = lastScore - margin;
            beta = lastScore + margin;
            currentDepthScore = Search(pos, 0, alpha, beta, depth, false);

            if (State.isStopping) {
                break;
            }

            // we have finished within the window, break the loop

            if (currentDepthScore > alpha && currentDepthScore < beta) {
                return currentDepthScore;
            }

            // verify checkmate by searching with infinite bounds

            if (currentDepthScore > EvalLimit) {
                break;
            }
        }
    }

    // full window search

    return Search(pos, 0, -Infinity, Infinity, depth, false);
    
}
