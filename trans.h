// Publius - Didactic public domain bitboard chess engine by Pawel Koziol

#pragma once

// bound types

enum eHashEntry { None, upperBound, lowerBound, exactEntry };

// transposition table record

typedef struct {
    Bitboard key;
    short move;
    short score;
    short date;
    unsigned char flags;
    unsigned char depth;
} hashRecord;


const int numberOfBuckets = 4;

// transposition table class

class TransTable {
private:
    hashRecord* table;
    int tableSize;
    int tt_date;
    int ScoreFromTT(int score, int ply);
    int ScoreToTT(int score, int ply);
    hashRecord* FindFirstSlot(Bitboard key);
public:
    void Clear(void);
    void Age(void);
    void Allocate(int mbsize);
    bool Retrieve(Bitboard key, Move* move, int* score, int* flag, int alpha, int beta, int depth, int ply);
    void Store(Bitboard key, Move move, int score, int flags, int depth, int ply);
    void Exit(void);
};

extern TransTable TT;