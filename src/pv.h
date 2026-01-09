// Publius - Didactic public domain bitboard chess engine by Pawel Koziol

#pragma once

class PvCollector {
public:
    void Clear();
    void Update(int ply, Move move);
    void SendBestMove();
    Move GetBestMove();
    void RememberBestLine();
    void Overwrite(Move move);
    void Display(int score, int bound);
    std::string GetOutputStringWithoutDepth(int score, int bound);
    std::string GetTimeString();
    std::string GetScoreString(int score, int bound);
    std::string GetPvString();
    std::string oldPvString = "";

    // array keeping principal variation, also
    // used to retrieve best move and ponder move
    // and to print the main line
    Move line[SearchTreeSize + 2][SearchTreeSize + 2];
    int size[SearchTreeSize + 2];
};

extern PvCollector Pv;