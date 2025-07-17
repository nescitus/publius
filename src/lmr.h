// Publius - Didactic public domain bitboard chess engine by Pawel Koziol

#pragma once

class LmrData {
public:
    LmrData(); // constructor
    int table[2][64][64];
};

extern LmrData Lmr;