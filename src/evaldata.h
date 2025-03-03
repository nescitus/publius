#pragma once

class EvalData {
public:
	int mg[2];
	int eg[2];
	Bitboard enemyKingZone[2];
	int minorAttacks[2];
	int rookAttacks[2];
	int queenAttacks[2];
	int phase; // game phase (24 for starting position)
	void Clear();
	void Add(Color color, int mgVal, int egVal);
};