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

int oldEval[PlyLimit];

int Search(Position *pos, int ply, int alpha, int beta, int depth, bool wasNull) {

    int bestScore, newDepth, eval, moveListLength;
    int isInCheck, hashFlag, reduction, score, moveType;
    int move, ttMove, bestMove, movesTried, quietMovesTried;
    bool isRoot, isPv;
    EvalData e;
    MoveList list;
    int moveList[256];

    // Init

    move = 0;
    ttMove = 0;
    bestMove = 0;
    movesTried = 0;
    quietMovesTried = 0;
    isRoot = !ply;

    // We distinguish two kinds of nodes:
    // zero window nodes and principal variation
    // nodes. Zero window nodes can only fail high
    // or fail low, as there is no distance between
    // alpha and beta. Only pv-nodes can return
    // an exact score.
    isPv = (beta > alpha + 1);

    // QUIESCENCE SEARCH entry point. Ideally
    // we want to evaluate a quiet position
    // (i.e. a position where there are no
    // favourable captures, so that we may
    // consider its evaluation stable). That's
    // why at leaf nodes we initiate 
    // a capture-only search instead of
    // returning the evaluation score here. 

    if (depth <= 0) {
        return Quiesce(pos, ply, 0, alpha, beta);
    }

    // Some bookkeeping
    nodeCount++;
    Pv.size[ply] = ply;

    // Periodically check for timeout, 
    // ponderhit or stop command
    TryInterrupting();

    // Exit to unwind search if it has timed out
    if (State.isStopping) {
        return 0;
    }

    // Quick exit on on a statically detected draw, 
    // unless we are at root, where we need to have
    // a move.
    if (pos->IsDraw() && !isRoot) {

        // Too many early exits in a row 
        // might cause a timeout, so we safeguard
        if (Timeout())
            State.isStopping = true;

        return ScoreDraw;
    }

    // MATE DISTANCE PRUNING, a minor improvement 
    // shaving off some nodes when the checkmate 
    // is near. It prevents looking for the longer
    // checmkates if a shorter one has been already 
    // found. It cannot be used at the root, as it 
    // doesn't return a move, only a value.

    if (!isRoot) {
        alpha = std::max(alpha, -MateScore + ply);
        beta = std::min(beta, MateScore - ply + 1);
        if (alpha >= beta) {
            return alpha;
        }
    }

    //  READ THE TRANSPOSITION TABLE. If we have 
    //  already searched the current position to 
    //  sufficient depth, we may use the score
    //  of the pasr search directly. If the depth 
    //  was lower, we still expect the move from 
    //  the previous search to be good and we will
    //  try it first.
    
    if (TT.Retrieve(pos->boardHash, &ttMove, &score, &hashFlag, alpha, beta, depth, ply)) {

        // Remember that pv-nodes don't use some 
        // pruning/reduction techniques. Because 
        // of that, we cannot use score from the
        // zero  window nodes. Despite the  same
        // nominal depth, they represent more
        // shallow and less precise search.
        if (!isPv || (score > alpha && score < beta)) {
            return score;
        }
    }

    // Safeguard against ply limit overflow
    if (ply >= PlyLimit - 1) {
        return Evaluate(pos, &e);
    }

    // Are we in check? Knowing that is useful 
    // for pruning/reduction/extension decisions
    isInCheck = pos->IsInCheck();

    // Init eval and improving flag.
    // Nodes where the side to move 
    // is not improving the eval are
    // probably less interesting and
    // warrant more pruning.

    bool improving = true; // default - less pruning

    // Evaluate position, unless in check
    if (isInCheck)
        eval = -Infinity;
    else
        eval = Evaluate(pos, &e);

    // Save eval for the current ply.
    oldEval[ply] = eval;

    // We check whether the eval has
    // deteriorated from two ples ago.
    // If so, we will prune and reduce 
    // a bit more.
    if (ply > 1) {
        if (oldEval[ply - 2] > eval)
            improving = false;
    }

    // NODE-LEVEL PRUNING. We try to avoid searching
    // the current node. All the techniques used for it
    // are speculative, but statistically they work.
    //
    // We skip node level pruning after a null move,
    // in check, in pv-nodes and in the late endgame.
    if (!wasNull &&
       !isInCheck &&
       !isPv &&
        pos->CanTryNullMove()) 
    {

        // STATIC NULL MOVE (or Reverse Futility Pruning 
        // or Beta Pruning) is similar to null move.
        // Instead of letting the opponent search
        // two moves in a row, it simply assumes some
        // loss, increasing with depth. If side to move
        // can accept that loss, then we prune.

        if (depth <= 4) {
            score = eval - 125 * depth;
            if (score > beta)
                return score;
        }

        // RAZORING - we drop directly to the quiescence
        // search if eval score is really bad. ~8 Elo gain.
        if (!ttMove && depth <= 3) {
            int threshold = beta - 300 + 60 * depth;
            if (eval < threshold) {
                score = Quiesce(pos, ply, alpha, beta, 0);
                if (score < threshold) {
                    return score;
                }
            }
        }

        // NULL MOVE PRUNING means allowing the opponent
        // to execute two moves in a row, for eample 
        // capturing something and escaping a recapture. 
        // If this cannot  wreck our position, then it is 
        // so good that there's  no  point in searching 
        // any further. We cannot really do two null moves 
        // in a row, as this would be the same position
        // searched at the smaller depth. "wasNull" flag
        // above takes care of that. Also, null move is 
        // not used in the endgame because of the risk 
        // of zugzwang - se CanTryNullMove() function 
        // for details.

        if (eval > beta && depth > 1) {

            // Set null move reduction
            reduction = 3 + depth / 6;
            if (eval - beta > 200) reduction++;

            // Do null move search, giving the opponent
            // two moves in a row
            pos->DoNull(ply);
            score = -Search(pos, ply + 1, -beta, -beta + 1, depth - reduction, true);
            pos->UndoNull(ply);

            if (State.isStopping)
                return 0;

            // NULL MOVE VERIFICATION - at higher depths
            // we perform a normal search to a reduced
            // depth. The idea is to have some safeguard
            // against zugzwangs.
            if (depth - reduction > 5 && score >= beta)
                score = Search(pos, ply, alpha, beta, depth - reduction - 4, true);
            
            if (State.isStopping) 
                return 0;

            if (score >= beta) {
                return score;
            }
        }   // end of null move code
    }

    // SET FUTILITY PRUNING FLAG. We are going to bet
    // that if the static evaluation of a node is bad,
    // then quiet moves will lead to no improvement.
    // Score margin is increased with depth.

    bool canDoFutility = false;

    if (depth <= 6 &&
        !isInCheck &&
        !isPv &&
        eval + 75 * depth < beta) {
        canDoFutility = true;
    }

    // Reduce when position is not on transposition table.
    // Idea from Prodeo chess engine (from Ed Schroder).
    // Please note that the implementation is non-standard,
    // as normally pv-nodes are not excluded, but this is
    // what worked for this engine.
    if (depth > 5 && !isPv && ttMove == 0 && !isInCheck) {
        depth--;
    }

    // Init moves and variables before entering main loop
    bestScore = -Infinity;
    list.Clear();
    FillCompleteList(pos, &list);
    moveListLength = list.GetInd();

    // Calculate moves' scores to sort them. Normally
    // we are going to search the transposition table
    // move first; in root node we start searching
    // from the best move from the previous iteration.
    if (isRoot) 
        list.ScoreMoves(pos, ply, Pv.line[0][0]);
    else      
        list.ScoreMoves(pos, ply, ttMove);

    // Check extension
    if (isInCheck && (isPv || depth < 7)) depth++;

    // Main loop
    if (moveListLength) {
        for (int i = 0; i < moveListLength; i++) {

            move = list.GetMove();
            moveType = GetMoveType(pos, move, ttMove, ply);

            pos->DoMove(move, ply);

            // Filter out illegal moves
            if (pos->LeavesKingInCheck()) {
                pos->UndoMove(move, ply);
                continue;
            }

            // Update move statistics
            moveList[movesTried] = move;
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

            // Late move pruning. Near the leaf nodes
            // quiet moves that are ordered way back
            // are unlikely to succeed, so we prune them.
            // This may lead to an error, but statistically
            // speaking, depth gain is more important
            // and a deeper search will fix the error.
            if (depth <= 3 &&
               !isPv && 
               !isInCheck &&
               !pos->IsInCheck() &&
                moveType == moveQuiet && 
                quietMovesTried > (3 + improving) * depth) 
            {
                pos->UndoMove(move, ply);
                continue;
            }

            // Late move reduction (LMR). We assume
            // that with decent move ordering cutoffs
            // will be caused by the moves tried early on.
            // That's why we search later moves at the 
            // reduced depth. However, if a reduced depth
            // search scores above beta, we need to search
            // at the normal depth.
            if (depth > 1 && 
                quietMovesTried > 3 && 
                moveType == moveQuiet && 
               !isInCheck && 
               !pos->IsInCheck())
            {   
                reduction = Lmr.table[isPv]
                                     [std::min(depth,63)]
                                     [std::min(movesTried, 63)];

                // for now it is redundant
                // but as you add more conditions,
                // it will come handy
                reduction = std::min(reduction, newDepth - 1);

                // do a reduced depth search
                if (reduction > 0) {
                    score = -Search(pos, ply + 1, -alpha - 1, -alpha, newDepth - reduction, false);
                    
                    // if the reduced search score falls
                    // below alpha, don't bother with
                    // full depth search
                    if (score <= alpha) {
                        pos->UndoMove(move, ply);
                        if (State.isStopping) return 0;
                        continue;
                    }
                }
            }

            // PVS (Principal variation search)
            // we search the first move of a pv-node
            // with a full window. For each later node
            // de do a scout search first, testing whether
            // score is above alpha (and thus above last best score).
            // Only if it is, we do a full window search
            // to get the exact value of the current node.
            // Zero window searches are still conducted
            // with a zero window.
            if (bestScore == -Infinity)
                score = -Search(pos, ply + 1, -beta, -alpha, newDepth, false);
            else {
                score = -Search(pos, ply + 1, -alpha - 1, -alpha, newDepth, false);
                if (!State.isStopping && score > alpha)
                    score = -Search(pos, ply + 1, -beta, -alpha, newDepth, false);
            }

            // Undo move
            pos->UndoMove(move, ply);
            if (State.isStopping) {
                return 0;
            }

            // Beta cutoff
            if (score >= beta) {
                
                // Beta cutoff means that a move is good.
                // Update history table and killer moves
                // so that the move will be sorted higher
                // next time we encounter it.
                History.Update(pos, move, depth, ply);
                for (int i = 0; i < movesTried; i++) {
                    History.UpdateTries(pos, moveList[i], depth);
                }

                // Store move in the transposition table
                TT.Store(pos->boardHash, move, score, upperBound, depth, ply);

                // If beta cutoff occurs at the root, 
                // change the best move and display
                // the new mainline. (Cutoffs can happen
                // in the root node because er are using
                // the aspiration window).
                if (isRoot) {
                    Pv.Refresh(ply, move);
                    DisplayPv(score);
                }

                // Stop searching this node. It has already
                // refuted opponent's previous move and looking
                // for a better refutation would only waste time.
                return score;
            }

            // Updating score and alpha
            if (score > bestScore) {
                bestScore = score;

                // The current move is better than whatever
                // we have calculated so far. The lack of beta
                // cutoff implies that we are in the pv-node
                // with some distance between alpha and beta
                // and we are looking for the best move 
                // and its exact value.
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

    // Save score in the transposition table.
    // Please note that the search structure
    // ensures this is done only if we are not
    // unwinding the search due to timeout.
    // This is a common source of bugs. If you wish,
    // you can add an explicit test for that.
    if (bestMove) {
        TT.Store(pos->boardHash, bestMove, bestScore, exactEntry, depth, ply);
    } else {
        TT.Store(pos->boardHash, 0, bestScore, lowerBound, depth, ply);
    }

    return bestScore;
}

// We need to know the move type for the pruning decisions
int GetMoveType(Position *pos, int move, int ttMove, int ply) {

    if (move == ttMove) 
        return moveHash;   // move from the hash table

    if (IsMoveNoisy(pos, move))
        return moveNoisy;  // capture or promotion

    if (History.IsKiller(move, ply))
        return moveKiller; // killer move
   
    return moveQuiet;      // quiet move
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

    // print the main line
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

    // There are some commands
    // that need to be replied to during search
    if (InputAvailable()) {
 
        std::cin.getline(command, 4096);

        // user ordered us to stop
        if (!strcmp(command, "stop"))
            State.isStopping = true;

        // transition from pondering to normal search
        else if (!strcmp(command, "ponderhit"))
            State.isPondering = false;

        // ping equivalent in the UCI protocol
        else if (!strcmp(command, "isready"))
            std::cout << "readyok" << std::endl;
    }

    // the time is out!
    if (Timeout()) {
        State.isStopping = true;
    }
}

int Timeout() {

    return (!State.isPondering && 
            !Timer.IsInfiniteMode() && 
            Timer.TimeHasElapsed());
}
