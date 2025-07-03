#pragma once

enum {stageTT, stageGen, stageReturn};

class MovePicker {
public:
	Move tt;
	MoveList list;
	int stage;
	int listLength;
	int cnt;
	void Init(Move ttMove);
	Move Next(Position* pos, int ply);
};