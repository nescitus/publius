#pragma once

enum {stageTT, stageGen, stageReturn};

class MovePicker {
public:
	Move ttMove;
	MoveList list;
	int stage;
	int listLength;
	int cnt;
	void InitAllMoves(Move ttMove);
	Move NextMove(Position* pos, int ply);
};