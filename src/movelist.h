// Publius - Didactic public domain bitboard chess engine 
// by Pawel Koziol

#pragma once

constexpr int MovesLimit = 256;

class MoveList {
private:
    Move moves[MovesLimit];
    int values[MovesLimit];
    int ind;
    int get;
    void SwapMoves(const int i, const int j);
public:
    void Clear();
    void AddMove(Square fromSquare, Square toSquare, int flag);
    void AddMove(Move move);
    int GetLength();
    Move GetNextRawMove();
    Move GetBestMove();
    void ScoreNoisy(Position* pos);
    void ScoreQuiet(Position* pos);
};