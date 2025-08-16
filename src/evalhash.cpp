// Publius - Didactic public domain bitboard chess engine by Pawel Koziol

#include <iostream>
#include "types.h"
#include "position.h"
#include "score.h"
#include "evaldata.h"
#include "eval.h"

// Constructor definition
EvalHashTable::EvalHashTable(size_t size) : tableSize(size), EvalTT(new EvalTTEntry[size]) {
    if ((size & (size - 1)) != 0)
        throw std::invalid_argument("Table size must be a power of two.");
}

EvalHashTable::~EvalHashTable() {
    delete[] EvalTT;
}

void EvalHashTable::Save(Bitboard key, int val) {

    size_t addr = Hash(key);
    EvalTT[addr].key = key;
    EvalTT[addr].val = val;
}

bool EvalHashTable::Retrieve(Bitboard key, int* score) const {
    size_t addr = Hash(key);
    if (EvalTT[addr].key == key) {
        *score = EvalTT[addr].val;
        return true;
    }
    return false;
}

size_t EvalHashTable::Hash(Bitboard key) const {
    return key & (tableSize - 1); // Bitwise AND operation
}