// Publius - Didactic public domain bitboard chess engine 
// by Pawel Koziol

#include <cstring>
#include <iostream>
#include "types.h"
#include "bitboard.h"
#include "bitgen.h"
#include "limits.h"
#include "position.h"
#include "movelist.h"
#include "timer.h"
#include "history.h"
#include "trans.h"
#include "move.h"
#include "lmr.h"
#include "pv.h"
#include "evaldata.h"
#include "api.h"
#include "uci.h"
#include "movepicker.h"
#include "util.h"
#include "search.h"

const int singularDepth = 7;
Move dummyMove = CreateMove(A1, B8, 0); // clearly illegal
Move excludedMove = dummyMove;

// eval for each ply, to see if we are improving or not
int oldEval[PlyLimit];

int Search(Position* pos, int ply, int alpha, int beta, int depth, bool wasNullMove, bool isExcluded) {

    int bestScore, newDepth, eval, moveListLength, movesTried, quietMovesTried;
    int hashFlag, reduction, score, singularScore;
    Move move, ttMove, bestMove, singularMove;
    EvalData e;
    UndoData undo;
    MovePicker movePicker;
    Move listOfTriedMoves[256];
    bool singularExtension;

    // Init
    move = 0;
    ttMove = 0;
    bestMove = dummyMove;
    movesTried = 0;
    quietMovesTried = 0;
    singularExtension = false;
    singularScore = -Infinity;
    singularMove = 0;

    // Root node is different because
    // we need to record the best move
    const bool isRoot = !ply;

    // We distinguish two kinds of nodes:
    // zero window nodes and principal variation
    // nodes. Zero window nodes can only fail high
    // or fail low, as there is no distance between
    // alpha and beta. Only pv-nodes can return
    // an exact score.
    const bool isPv = (beta > alpha + 1);

    // QUIESCENCE SEARCH entry point. Ideally we want
    // to evaluate a quiet position (i.e. a position 
    // where there are no favourable captures, so that 
    // we may consider its evaluation stable). That's
    // why at leaf nodes we initiate a capture-only 
    // search instead of returning the evaluation score. 
    if (depth <= 0)
        return Quiesce(pos, ply, 0, alpha, beta);

    // Some bookkeeping
    Timer.nodeCount++;
    Pv.size[ply] = ply;

    // Periodically check for timeout, 
    // ponderhit or stop command
    TryInterrupting();

    // Exit to unwind search if it has timed out
    if (Timer.isStopping)
        return 0;

    // Quick exit on on a statically detected draw, 
    // unless we are at root, where we need to have
    // a move.
    if (pos->IsDraw() && !isRoot) {

        // Too many early exits in a row 
        // might cause a timeout, so we safeguard
        Timer.TryStoppingByTimeout();

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
        if (alpha >= beta)
            return alpha;
    }

    //  READ THE TRANSPOSITION TABLE. If we have 
    //  already searched the current position to 
    //  sufficient depth, we may use the score
    //  of the past search directly. If the depth 
    //  was lower, we still expect the move from 
    //  the previous search to be good and we will
    //  try it first.

    bool foundTTrecord = false;

    if (TT.Retrieve(pos->boardHash, &ttMove, &score, &hashFlag, alpha, beta, depth, ply)) {

        foundTTrecord = true;

        // Because pv-nodes don't use some pruning
        // or reduction techniques, we cannot always
        // reuse scores from the zero window nodes. 
        // Despite  the  same nominal  depth,  they
        // represent more shallow, less precise search.
        if (!isPv || (score > alpha && score < beta)) {
            if (!isExcluded)
                return score;
        }
    }

    // Safeguard against ply limit overflow
    if (ply >= PlyLimit - 1)
        return Evaluate(pos, &e);

    // Prepare for singular extension
    if (!isRoot &&                  // we are not at the root
        depth > singularDepth &&   // sufficient remaining depth
        excludedMove == dummyMove) // we are not in the singular search
    {

        if (TT.Retrieve(pos->boardHash, &singularMove, &singularScore, &hashFlag, alpha, beta, depth - 4, ply)) {

            // We have found upper bound hash entry
            // and it is not a checkmate score, so
            // we can try the singular extension.
            if ((hashFlag & lowerBound) && singularScore < EvalLimit)  
                singularExtension = true;
        }
    }

    // Are we in check? Knowing that is useful 
    // for pruning/reduction/extension decisions
    const bool isInCheckBeforeMoving = pos->IsInCheck();

    // Init eval and improving flag. Nodes where the side 
    // to move is not improving the eval are probably 
    // less interesting and warrant more pruning.

    // Evaluate position, unless in check
    eval = isInCheckBeforeMoving ? -Infinity : Evaluate(pos, &e);

    // Adjust node eval by using score from the trans-
    // position table. It modifies a few things, including 
    // null move probability (~20 Elo)
    if (foundTTrecord) {
        if (hashFlag & (score > eval ? upperBound : lowerBound))
            eval = score;
    }

    // Save eval for the current ply.
    oldEval[ply] = eval;

    // We check whether the eval has improved from 
    // two plies ago. As of now, it affects late move 
    // pruning only, but some more uses will be tested.
    const bool improving = SetImproving(eval, ply);

    // NODE-LEVEL PRUNING. We try to avoid searching
    // the current node. All the techniques used for it
    // are speculative, but statistically they work.
    //
    // We skip node level pruning after a null move,
    // when in check, in pv-nodes, in the late endgame
    // and in singular search.
    if (!wasNullMove &&
        !isInCheckBeforeMoving &&
        !isPv &&
        !isExcluded &&
        pos->CanTryNullMove())
    {

        // STATIC NULL MOVE (or Reverse Futility Pruning 
        // or Beta Pruning) is similar to null move.
        // Instead of letting the opponent search
        // two moves in a row, it simply assumes some
        // loss, increasing with depth. If side to move
        // can accept that loss, then we prune (~14 Elo).

        if (depth <= 6) {
            score = eval - 135 * depth;
            if (score > beta)
                return score;
        }

        // RAZORING - we drop directly to the quiescence
        // search if static eval is really bad. (~15 Elo)

        if (depth <= 3 && eval + 200 * depth < beta) {
            score = Quiesce(pos, ply, 0, alpha, beta);
            if (score < beta) // no fail high!
                return score;
        }

        // NULL MOVE PRUNING means allowing the opponent
        // to execute two moves in a row, for example 
        // capturing something and escaping a recapture. 
        // If this cannot  wreck our position, then it is 
        // so good that there's  no  point in searching 
        // any further. We cannot really do two null moves 
        // in a row, as this would be the same position
        // searched at the smaller depth. "wasNull" flag
        // above takes care of that. Also, null move is 
        // not used in the endgame because of the risk 
        // of zugzwang - see CanTryNullMove() function 
        // for details. (~82 Elo)

        if (eval > beta && depth > 1) {

            // Set null move reduction
            reduction = 3 + depth / 6 + (eval - beta > 200);

            // Do null move search, giving the opponent
            // two moves in a row
            pos->DoNull(&undo);
            score = -Search(pos, ply + 1, -beta, -beta + 1, depth - reduction, true, false);
            pos->UndoNull(&undo);

            if (Timer.isStopping)
                return 0;

            // NULL MOVE VERIFICATION - at higher depths
            // we perform a normal search to a reduced
            // depth. The idea is to have some safeguard
            // against zugzwangs (~10 Elo)
            if (depth - reduction > 5 && score >= beta)
                score = Search(pos, ply, alpha, beta, depth - reduction - 4, true, false);

            if (Timer.isStopping)
                return 0;

            if (score >= beta)
                return score;
        }   // end of null move code
    } // end of node pruning block

    // SET FUTILITY PRUNING FLAG. If the static 
    // evaluation of a node is bad, we bet that
    // quiet moves will lead to no improvement.
    // Score margin is increased with depth. Please
    // note that our implementation does not prune 
    // moves that give check, which is slightly unusual.
    // (~2 Elo, so definately needs tuning)

    bool canDoFutility = (depth <= 6 &&
        !isInCheckBeforeMoving &&
        !isPv &&
         eval + 75 * depth < beta);

    // INTERNAL ITERATIVE REDUCTION (non-standard).
    // Reduce when position is not on transposition table.
    // Idea from Prodeo chess engine (from Ed Schroder).
    // Please note that the implementation is non-standard,
    // as normally pv-nodes are not excluded, but this is
    // what worked for this engine. (~9 Elo)
    if (depth > 5 && !isPv && ttMove == 0 && !isInCheckBeforeMoving)
        depth--;

    // Init moves and variables before entering main loop
    bestScore = -Infinity;

    // Calculate moves' scores to sort them. Normally
    // we are going to search the transposition table
    // move first; in root node we start searching
    // from the best move from the previous iteration.
    movePicker.Init(modeAll,
                    isRoot ? Pv.line[0][0] : ttMove,
                    History.GetKiller1(ply), 
                    History.GetKiller2(ply));

    // Main loop

    while ((move = movePicker.NextMove(pos, ply)) != 0) {

        // In singular search we omit the best move
        // checking whether there are viable alternatives
        if (move == excludedMove && isExcluded)
            continue;

        // Detect if a move gives check (without playing it).
        // This is not a popular idea, but Koivisto does it.
        bool moveGivesCheck = pos->MoveGivesCheck(move);

        // Check extension
        bool doExtension = moveGivesCheck && (isPv || depth < 4);

        // Singular extension: tried once per search
        if (depth > singularDepth &&
           !doExtension &&
            singularMove &&
            move == singularMove && // we are about to search the best move from tt
            singularExtension &&    // conditions for the singular search are met
            excludedMove == dummyMove) {

            // Move from the transposition table
            // might be a singular move. We are
            // trying to disprove it, looking for 
            // moves that  comes close to it. If 
            // there are none, we will extend.
            int newAlpha = -singularScore - 50;

            // We are checking for decent alternatives,
            // so we do not test the singular move
            // candidate.
            excludedMove = move;

            // The only instance when we search with
            // isExcluded flag set to "true". The flag
            // switches off all the node-level pruning
            // techniques, including reading score from
            // the transposition table. After all, to 
            // test alternatives to the singular move, 
            // we need to search them. We also refrain
            // from saving the result of this search
            // in the transposition table, because
            // it actively avoids searching the best move.
            int sc = Search(pos, ply + 1, newAlpha, newAlpha + 1, (depth - 1) / 2, false, true);
            excludedMove = dummyMove;

            if (Timer.isStopping)
                return 0;

            // No move scores close to the singular
            // move, so we extend. The search result
            // relies on a single move - it would be
            // a shame if a deeper search revealed
            // a refutation.
            if (sc <= newAlpha)
                doExtension = true;
        } // end of singular extension code

        // Check basic conditions for pruning a move
        bool canPruneMove = !isPv && !isInCheckBeforeMoving &&
            !moveGivesCheck && movePicker.currentMoveStage == stageReturnQuiet;

        // FUTILITY PRUNING
        // (~2 Elo, so definately needs tuning)
        if (canDoFutility && movesTried > 0 && canPruneMove)
            continue;

        // LATE MOVE PRUNING. At low depths quiet moves 
        // near the end of the move list are unlikely to 
        // succeed, so we prune them. This may lead to an 
        // error, but usually depth gain is more important
        // and  a  deeper search will fix the  error.  Our 
        // implementation is slightly unusual, because it 
        // avoids pruning moves that give check. (~90 Elo)
        if (depth <= 3 &&
            canPruneMove &&
            quietMovesTried > ((3 + improving) * depth) - 1)
        {
            continue;
        }

        // SEE PRUNING. At low depths bad captures are
        // unlikely to succeed, so we prune them. Static
        // exchange evaluation margin increases with depth.
        //if (movePicker.stage == stageReturnBad &&
        //    depth < 4 && !isPv &&
        //   !isInCheckBeforeMoving &&
        //   !moveGivesCheck &&
        //    alpha > -MateScore + 500 &&
        //    movesTried > 1) 
        //{
        //    if (Swap(pos, GetFromSquare(move), GetToSquare(move)) < -25 * depth * depth)
        //        continue;
        //}

        // Make move
        pos->DoMove(move, &undo);

        // Filter out illegal moves
        if (pos->IsOwnKingInCheck()) {
            pos->UndoMove(move, &undo);
            continue;
        }

        // Update move statistics
        listOfTriedMoves[movesTried] = move;
        movesTried++;
        if (movePicker.currentMoveStage == stageReturnQuiet)
            quietMovesTried++;

        // Set new search depth
        newDepth = depth - 1 + doExtension;

        // LATE MOVE REDUCTION (LMR). We assume that
        // with decent move ordering early moves are
        // much more likely to cause a cutoff. That's
        // why we search later moves at the  reduced
        // depth. However, if a reduced depth search
        // scores  above beta, we need to  re-search
        // at the normal depth (~125 Elo)
        if (depth > 1 &&
            quietMovesTried > 3 &&
            movePicker.currentMoveStage == stageReturnQuiet &&
            !isInCheckBeforeMoving &&
            !moveGivesCheck)
        {
            reduction = Lmr.table[isPv]
                                 [std::min(depth, 63)]
                                 [std::min(movesTried, 63)];

            // TODO: increase reduction when not improving
            //if (reduction > 1 && improving) 
            //    reduction--;
            
            // Reduction cannot exceed actual depth
            reduction = std::min(reduction, newDepth - 1);

            // do a reduced depth search
            if (reduction > 0) {
                score = -Search(pos, ply + 1, -alpha - 1, -alpha, newDepth - reduction, false, false);

                // If  the reduced search score falls  below
                // alpha, don't bother with full depth search
                if (score <= alpha) {
                    pos->UndoMove(move, &undo);
                    if (Timer.isStopping)
                        return 0;
                    continue;
                }
            }
        }

        // PVS (Principal variation search). We search 
        // the first move of a pv-node with a full window. 
        // For each later move we do a scout search first, 
        // testing whether score is above alpha (and thus 
        // above last best score). Only if it is, we do 
        // a full window search to get the exact value 
        // of the current node. Zero window searches are 
        // still conducted with a zero window.

        if (bestScore == -Infinity)
            score = -Search(pos, ply + 1, -beta, -alpha, newDepth, false, false);
        else {
            score = -Search(pos, ply + 1, -alpha - 1, -alpha, newDepth, false, false);
            if (!Timer.isStopping && score > alpha)
                score = -Search(pos, ply + 1, -beta, -alpha, newDepth, false, false);
        }

        // Undo move
        pos->UndoMove(move, &undo);

        if (Timer.isStopping)
            return 0;

        // Beta cutoff
        if (score >= beta) {

            // Beta cutoff means that a move is good.
            // Update history table and killer moves
            // so that the move will be sorted higher
            // next time we encounter it.
            History.Update(pos, move, depth, ply);
            for (int i = 0; i < movesTried; i++)
                History.UpdateTries(pos, listOfTriedMoves[i], depth);

            // Store move in the transposition table
            if (!isExcluded)
                TT.Store(pos->boardHash, move, score, lowerBound, depth, ply);

            // If beta cutoff occurs at the root, 
            // change the best move and display
            // the new mainline. (cutoffs can happen
            // in the root node because we are using
            // the aspiration window).
            if (isRoot) {
                Pv.Update(ply, move);
                Pv.Display(score);
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
                Pv.Update(ply, move);
                if (isRoot)
                    Pv.Display(score);
            }
        }

    } // end of the main loop

    // Return correct checkmate/stalemate score
    if (bestScore == -Infinity)
        return pos->IsInCheck() ? -MateScore + ply : 0;

    // Save score in the transposition table.
    // Please note that the search structure
    // ensures this is done only if we are not
    // unwinding the search due to timeout.
    // This is a common source of bugs. If you 
    // wish, add an explicit test for that.
    if (!isExcluded) {
        if (bestMove)
            TT.Store(pos->boardHash, bestMove, bestScore, exactEntry, depth, ply);
        else
            TT.Store(pos->boardHash, 0, bestScore, upperBound, depth, ply);
    }

    return bestScore;
}

bool SetImproving(int eval, int ply) {
    return !(ply > 1 && oldEval[ply - 2] > eval);
}

void TryInterrupting(void) {

    char command[80];

    // We don't check for timeout in every node,
    // but only every so often, to improve speed
    if (Timer.nodeCount & 4095 || Timer.rootDepth == 1)
        return;

    // Search limited by the nodecount
    // (not entirely precise, see previous comment)
    Timer.TryStoppingByNodecount();

    // There are some commands
    // that need to be replied to during search
    if (InputAvailable()) {

        std::cin.getline(command, 4096);

        // user ordered us to stop
        if (!strcmp(command, "stop"))
            OnStopCommand();

        // transition from pondering to normal search
        else if (!strcmp(command, "ponderhit"))
            Timer.isPondering = false;

        // ping equivalent in the UCI protocol
        else if (!strcmp(command, "isready"))
            std::cout << "readyok" << std::endl;
    }

    // check if the time is out
    Timer.TryStoppingByTimeout();
}
