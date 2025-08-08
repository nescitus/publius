// Publius - Didactic public domain bitboard chess engine by Pawel Koziol

#pragma once

// bound types

enum eHashEntry { None, upperBound, lowerBound, exactEntry };

// transposition table record

typedef struct {
    Bitboard key;
    short move;
    short score;
    unsigned char flags;
    unsigned char depth;
} hashRecord;

// transposition table class

class TransTable {
private:
    hashRecord* table;
    int tableSize;
    int ScoreFromTT(int score, int ply);
    int ScoreToTT(int score, int ply);
    hashRecord* FindSlot(Bitboard key);
public:
    void Clear(void);
    void Allocate(int mbsize);
    bool Retrieve(Bitboard key, Move* move, int* score, int* flag, int alpha, int beta, int depth, int ply);
    void Store(Bitboard key, Move move, int score, int flags, int depth, int ply);
    void Exit(void);
};

extern TransTable TT;