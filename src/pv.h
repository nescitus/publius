#pragma once

class PvCollector {
public:
	void Clear();
	void Refresh(int ply, Move move);
	void SendBestMove();
	void PrintMainLine();

	// array keeping principal variation, 
	// used also to retrieve best move and ponder move
	// and to print main line
	Move line[PlyLimit + 2][PlyLimit + 2];
	int size[PlyLimit + 2];
};

extern PvCollector Pv;