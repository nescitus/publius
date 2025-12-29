// Publius - Didactic public domain bitboard chess engine 
// by Pawel Koziol

#pragma once

#include <cstdint>

// Define the eval hash entry
struct EvalTTEntry {
    Bitboard key;
    int val;
};

class EvalHashTable {
public:
    EvalHashTable(size_t size); // constructor
    ~EvalHashTable(); // destructor
    void Clear();
    void Save(Bitboard key, int val);
    bool Retrieve(Bitboard key, int* score) const;

private:
    size_t Address(Bitboard key) const;
    const size_t tableSize; // Size of the hash table (must be a power of two)
    EvalTTEntry* EvalTT;    // Dynamically allocated array
};

extern EvalHashTable EvalHash; // full evaluation hashtable
extern EvalHashTable PawnHash; // pawn structure eval hashtable

// Main evaluation functions
int EvalNN(Position* pos);
int EvalHCE(Position* pos, EvalData* e);

// Handcrafted evaluation helpers
void EvalBasic(EvalData* e, const Color color, const PieceType piece, const int sq);
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

