// Publius - Didactic public domain bitboard chess engine by Pawel Koziol

#pragma once

class HistoryData {
private:
    Move killer1[PlyLimit];
    Move killer2[PlyLimit];
    int cutoffHistory[12][64];
    int triesHistory[12][64];
    Square lastCaptureSquare[PlyLimit];
    int Inc(const int depth);
public:
    HistoryData(); // constructor
    void Clear(void);
    void Trim(void);
    void Update(Position* pos, const Move move, const int depth, const int ply);
    void UpdateTries(Position* pos, const Move move, const int depth);
    int GetScore(Position* pos, const Move move);
    Move GetKiller1(const int ply);
    Move GetKiller2(const int ply);
    bool IsKiller(const Move move, const int ply);
};

extern HistoryData History;