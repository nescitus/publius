// Publius - Didactic public domain bitboard chess engine by Pawel Koziol

#pragma once

#include <cstdint>

// Define the pawn hash entry

struct sPawnHashEntry {
    Bitboard key;
    int val[2];
};

// Define the eval hash entry
struct EvalTTEntry {
    Bitboard key;
    int val;
};

const int PAWN_HASH_SIZE = 16384;

extern sPawnHashEntry PawnTT[PAWN_HASH_SIZE];

class EvalHashTable {
public:
    EvalHashTable(size_t size); // constructor
    ~EvalHashTable(); // destructor
    void Save(Bitboard key, int val);
    bool Retrieve(Bitboard key, int* score) const;

private:
    size_t Hash(Bitboard key) const;
    EvalTTEntry* EvalTT;    // Dynamically allocated array
    const size_t tableSize; // Size of the hash table (must be a power of two)
};

extern EvalHashTable EvalHash;

void EvalBasic(EvalData* e, const Color color, const int piece, const int sq);
void EvalPawnStructure(const Position* pos, EvalData* e);
void EvalPawn(const Position* pos, EvalData* e, Color color);
void EvalKnight(const Position* pos, EvalData* e, Color color);
void EvalBishop(const Position* pos, EvalData* e, Color color);
void EvalRook(const Position* pos, EvalData* e, Color color);
void EvalQueen(const Position* pos, EvalData* e, Color color);
void EvalKing(const Position* pos, EvalData* e, Color color);
void EvalKingAttacks(EvalData* e, Color color);
void EvalPasser(const Position* pos, EvalData* e, Color color);
void EvalPressure(Position* pos, EvalData* e, Color side);
int GetDrawMul(Position* pos, const Color strong, const Color weak);
int Interpolate(EvalData* e);

