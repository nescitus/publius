// Publius - Didactic public domain bitboard chess engine by Pawel Koziol

#pragma once

//#define USE_TUNING

// REGEX to count all the lines under MSVC 13: ^(?([^\r\n])\s)*[^\s+?/]+[^\n]*$
// 3987 lines

#include <iostream>
#include <algorithm>

// list

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
    int GetInd();
    Move GetNextRawMove();
    Move GetBestMove();
    void ScoreNoisy(Position* pos);
    void ScoreQuiet(Position* pos);
};

bool IsBadCapture(Position* pos, Move move);
int Swap(const Position* pos, const Square fromSquare, const Square toSquare);

int InputAvailable(void);
void OnNewGame(void);
std::string SquareName(Square sq);

// diagnostics

void Bench(Position* pos, int depth);
void PrintBoard(Position* pos);
Bitboard Perft(Position* pos, int ply, int depth, bool isNoisy);
void PrintBitboard(Bitboard b);

extern int rootDepth;
extern Bitboard nodeCount;

static const int castleMask[64] = {
    13, 15, 15, 15, 12, 15, 15, 14,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    7,  15, 15, 15,  3, 15, 15, 11,
};

std::tuple<Color, int> PieceFromChar(char c);