#pragma once

class PvCollector {
public:
	void Clear();
	void Refresh(int ply, int move);
	// array keeping principal variation, used also to retrieve best move and ponder move
	int line[PlyLimit + 2][PlyLimit + 2];
	int size[PlyLimit + 2];
};

extern PvCollector Pv;