// Publius - Didactic public domain bitboard chess engine by Pawel Koziol

#pragma once

class Parameters {
public:
    void Init();
    int pawnSupport[2][64];
    int pst[2][6][64];
    void PrintPst(int piece);
};

extern Parameters Params;