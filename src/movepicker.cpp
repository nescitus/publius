#include "types.h"
#include "limits.h"
#include "publius.h"
#include "legality.h"
#include "movepicker.h"

bool isUsingTT = true;

void MovePicker::Init(Move ttMove) {
	tt = ttMove;
	list.Clear();
	if (isUsingTT) 
		stage = stageTT;
	else
	    stage = stageGen;
}

Move MovePicker::Next(Position* pos, int ply) {
	Move move;

	while (true) {
		switch (stage) {

		case stageTT:
			stage = stageGen;
			if (IsPseudoLegal(pos, tt))
				return tt;
			break;

		case stageGen:
			FillCompleteList(pos, &list);
			list.ScoreMoves(pos, ply, tt);
			listLength = list.GetInd();
			cnt = 0;
			stage = stageReturn;
			break;

		case stageReturn:
			while (cnt < listLength) {
				move = list.GetMove();
				cnt++;
				if (move == tt && isUsingTT)
					continue;  // Avoid returning tt again
				return move;
			}
			return 0;
		}
	}
}

// isUsingTT == false:
// Bench at depth 15 took 26579 milliseconds, searching 35599827 nodes at 1339396 nodes per second.
// isUsingTT == true:
// Bench at depth 15 took 25656 milliseconds, searching 34483645 nodes at 1344077 nodes per second.