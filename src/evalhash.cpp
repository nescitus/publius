// Publius - Didactic public domain bitboard chess engine 
// by Pawel Koziol

// Evaluation hash table saves the evaluation
// of position to speed up the engine.

#include <iostream>
#include "types.h"
#include "position.h"
#include "score.h"
#include "evaldata.h"
#include "eval.h"

// Constructor
EvalHashTable::EvalHashTable(size_t size) : tableSize(size), EvalTT(new EvalTTEntry[size]) {
    if ((size & (size - 1)) != 0)
        throw std::invalid_argument("Table size must be a power of two.");
}

// Destructor
EvalHashTable::~EvalHashTable() {
    delete[] EvalTT;
}

// save position evaluation
void EvalHashTable::Save(Bitboard key, int val) {

    size_t addr = Address(key);
    EvalTT[addr].key = key;
    EvalTT[addr].val = val;
}

// retrieve position evaluation
bool EvalHashTable::Retrieve(Bitboard key, int* score) const {
    size_t addr = Address(key);
    if (EvalTT[addr].key == key) {
        *score = EvalTT[addr].val;
        return true;
    }
    return false;
}

// where to save/look for data?
size_t EvalHashTable::Address(Bitboard key) const {
    return key & (tableSize - 1); // Bitwise AND operation
}