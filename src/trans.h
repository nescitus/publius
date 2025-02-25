#pragma once

// transposition table

typedef struct {
	Bitboard key;
	short move;
	short score;
	unsigned char flags;
	unsigned char depth;
} hashRecord;

class TransTable {
private:
	hashRecord* table;
	int tableSize;
public:
	void Clear(void);
	void Allocate(int mbsize);
	bool Retrieve(Bitboard key, int* move, int* score, int* flag, int alpha, int beta, int depth, int ply);
	void Store(Bitboard key, int move, int score, int flags, int depth, int ply);
	void Exit(void);
};

extern TransTable TT;