#include "color.h"
#include "square.h"
#include "publius.h"
#include "move.h"
#include "search.h"

int Quiesce(Position *pos, int ply, int alpha, int beta) {

  int best, move, score;
  EvalData e;
  MoveList list;

  // Statistics and attempt at quick exit

  nodeCount++;
  TryInterrupting();
  if (State.isStopping) {
      return 0;
  }

  pvSize[ply] = ply;
  
  // Draw detection

  if (pos->IsDraw()) {
      return 0;
  }

  // Safeguarding against overflow
  
  if (ply >= PlyLimit - 1) {
      return Evaluate(pos, &e);
  }

  // Get a stand-pat score and adjust bounds
  // (exiting if eval exceeds beta)

  best = Evaluate(pos, &e);
  
  if (best >= beta) {
      return best;
  }

  if (best > alpha) {
      alpha = best;
  }

  // Generate and sort move list

  list.Clear();
  FillNoisyList(pos, &list);
  int length = list.GetInd();
  list.ScoreMoves(pos, ply, 0);

  // Main loop

  if (length) {

	  for (int i = 0; i < length; i++) {

		  move = list.GetMove();

		  // Delta pruning
          // TODO: verify

		  static const int delta[7] = { 400, 625, 625, 800, 1300, 0, 0 };

          if (best + delta[pos->PieceTypeOnSq(GetToSquare(move))] < alpha) {
              continue;
          }

          // Make move, unless illegal

		  pos->DoMove(move, ply);
		  if (pos->LeavesKingInCheck()) { 
              pos->UndoMove(move, ply); 
              continue; 
          }

          // Recursion

		  score = -Quiesce(pos, ply + 1, -beta, -alpha);

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
				  RefreshPv(ply, move);
			  }
		  }
	  }
  }

  return best;
}