#include "color.h"
#include "square.h"
#include "limits.h"
#include "publius.h"
#include "move.h"
#include "pv.h"
#include "evaldata.h"
#include "eval.h"
#include "search.h"

int Quiesce(Position *pos, int ply, int qdepth, int alpha, int beta) {

  int best, move, score;
  EvalData e;
  MoveList list;
  bool isInCheck;

  // Statistics and attempt at quick exit

  nodeCount++;

  // Check for timeout
  TryInterrupting();

  // Exit to unwind search if it has timed out
  if (State.isStopping) {
      return 0;
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
  // (exiting if eval exceeds beta)
  if (isInCheck)
      best = -Infinity;
  else
      best = Evaluate(pos, &e);
  
  if (best >= beta) {
      return best;
  }

  if (best > alpha) {
      alpha = best;
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
  list.ScoreMoves(pos, ply, 0);

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
              return score;
          }

		  // Adjust alpha and score
		  if (score > best) {
			  best = score;
			  if (score > alpha) {
				  alpha = score;
				  Pv.Refresh(ply, move);
			  }
		  }
	  }
  }

  // Return correct checkmate/stalemate score
  if (best == -Infinity) {
      return pos->IsInCheck() ? -MateScore + ply : 0;
  }

  return best;
}