#include "color.h"
#include "square.h"
#include "limits.h"
#include "publius.h"
#include "move.h"
#include "pv.h"
#include "evaldata.h"
#include "eval.h"
#include "search.h"
#include "trans.h"

int Quiesce(Position *pos, int ply, int qdepth, int alpha, int beta) {

  int bestScore, move, bestMove, ttMove = 0, hashFlag, score;
  EvalData e;
  MoveList list;
  bool isInCheck;
  bool isPv = (beta > alpha + 1);

  // Statistics and attempt at quick exit

  nodeCount++;

  // Check for timeout
  TryInterrupting();

  // Exit to unwind search if it has timed out
  if (State.isStopping) {
      return 0;
  }

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

  // Safeguarding against overflow
  if (ply >= PlyLimit - 1) {
      return Evaluate(pos, &e);
  }

  isInCheck = pos->IsInCheck();

  // Get a stand-pat score and adjust bounds
  // (exiting if eval exceeds beta
  // but starting at the lowers possible value
  // when in check)
  if (isInCheck)
      bestScore = -Infinity;
  else
      bestScore = Evaluate(pos, &e);
  
  if (bestScore >= beta) {
      return bestScore;
  }

  if (bestScore > alpha) {
      alpha = bestScore;
  }

  // Generate and sort move list
  list.Clear();
  if (isInCheck)
      FillCompleteList(pos, &list);
  else if (qdepth == 0)
      FillChecksAndCaptures(pos, &list);
  else
      FillNoisyList(pos, &list);

  int length = list.GetInd();
  list.ScoreMoves(pos, ply, ttMove);

  // Main loop
  if (length) {

	  for (int i = 0; i < length; i++) {

		  move = list.GetMove();

          if (!isInCheck && !pos->IsEmpty(GetToSquare(move))) {
              if (IsBadCapture(pos, move))
                  continue;
          }

          // Make move, unless illegal

		  pos->DoMove(move, ply);
		  if (pos->LeavesKingInCheck()) { 
              pos->UndoMove(move, ply); 
              continue; 
          }

          // Recursion
		  score = -Quiesce(pos, ply + 1, qdepth+1, -beta, -alpha);

          // Unmake move
		  pos->UndoMove(move, ply);
          
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

  if (bestMove) {
      TT.Store(pos->boardHash, bestMove, bestScore, exactEntry, 0, ply);
  }
  else {
      TT.Store(pos->boardHash, 0, bestScore, lowerBound, 0, ply);
  }

  return bestScore;
}