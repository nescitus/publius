// Publius - Didactic public domain bitboard chess engine by Pawel Koziol

#pragma once

class CorrHist {
private:
    int GetPawnKey(Position* pos);
    int pawnCorrhist[16384];
public:
    void Clear(void);
    void Update(Position* pos, int score, int eval, int depth);
    int Correct(Position* pos, int eval);
};

extern CorrHist Corrhist;