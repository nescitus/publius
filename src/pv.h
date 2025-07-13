#pragma once

class PvCollector {
public:
    void Clear();
    void Update(int ply, Move move);
    void SendBestMove();
    void Display(int score);

    // array keeping principal variation, also
    // used to retrieve best move and ponder move
    // and to print the main line
    Move line[PlyLimit + 2][PlyLimit + 2];
    int size[PlyLimit + 2];
};

extern PvCollector Pv;