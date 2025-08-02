// Publius - Didactic public domain bitboard chess engine by Pawel Koziol

#pragma once

const int corrhistSize = 16384;

class CorrHist {
private:
    int GetPawnKey(Position* pos);
    int pawnCorrhist[2][corrhistSize];
public:
    void Clear(void);
    void Update(Position* pos, int score, int eval, int depth);
    int Correct(Position* pos, int eval);
};

extern CorrHist Corrhist;