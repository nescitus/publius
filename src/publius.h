// Publius - Didactic public domain bitboard chess engine by Pawel Koziol

#pragma once

//#define USE_TUNING

// REGEX to count all the lines under MSVC 13: ^(?([^\r\n])\s)*[^\s+?/]+[^\n]*$
// 3987 lines

#include <iostream>
#include <algorithm>

static constexpr auto startFen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -";
static constexpr auto kiwipeteFen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -";

// We pack (mg, eg) score into a single integer.
// This gais some speed (by not adding the values
// twice), but more importantly simplifies the code.

constexpr int MakeScore(int mg, int eg) {
    return ((unsigned int)(eg) << 16) + mg;
}

constexpr int ScoreMG(int s) {
    return (int16_t)((uint16_t)((unsigned)(s)));
}

constexpr int ScoreEG(int s) {
    return (int16_t)((uint16_t)((unsigned)(s + 0x8000) >> 16));
}

#define S(mg, eg) MakeScore(mg, eg)

// data for undoing a move

typedef struct {
    Move move;
    int prey;
    int castleFlags;
    Square enPassantSq;
    int reversibleMoves;
    Bitboard boardHash;
    Bitboard pawnHash;
} UndoData;

// position

class Position {
private:
    Color sideToMove;
    Bitboard pieceBitboard[2][6];
    Square enPassantSq;
    Square kingSq[2];
    int castleFlags;
    int pieceCount[2][6];
    int pieceLocation[64];
    int reversibleMoves;
    int repetitionIndex;
    Bitboard repetitionList[256];

    void Clear();
    void SwitchSide();
    void ClearEnPassant();
    void MovePiece(const Color color, const int hunter, const Square fromSquare, Square toSquare);
    void MovePieceNoHash(const Color color, const int hunter, const Square fromSquare, Square toSquare);
    void AddPieceNoHash(const Color color, const int typeOfPiece, const Square square);
    void TakePiece(const Color color, const int typeOfPiece, const Square square);
    void TakePieceNoHash(const Color color, const int typeOfPiece, const Square square);
    void ChangePieceNoHash(const int oldType, const int newType, const Color color, const Square square);
    void SetEnPassantSquare(const Color color, Square toSquare);
    void UpdateCastlingRights(const Square fromSquare, const Square toSquare);
    Bitboard CalculateHashKey();
    Bitboard CalculatePawnKey();
    
    bool IsDrawByRepetition() const;
    bool IsDrawByInsufficientMaterial() const;
public:
    Bitboard boardHash;
    Bitboard pawnHash;
    bool SquareIsAttacked(const Square sq, const Color color) const;
    void Set(const std::string& str);
    void DoMove(const Move move, UndoData* undo);
    void DoNull(UndoData * undo);
    void UndoMove(const Move move, UndoData *undo);
    void UndoNull(UndoData *undo);
    bool IsDraw() const;
    bool CanTryNullMove(void) const;
    Color GetSideToMove() const;
    int GetPiece(const Square square) const;
    int Count(const Color color, const int type) const;
    int CountAllPawns() const;
    int CountMinors(const Color color) const;
    int CountMajors(const Color color) const;
    Bitboard Map(const Color color, const int piece) const;
    Bitboard Map(const Color color) const;
    bool IsOccupied(const Square sq) const;
    Bitboard Occupied() const;
    Bitboard Empty() const;
    Bitboard MapDiagonalMovers(const Color color) const;
    Bitboard MapStraightMovers(const Color color) const;
    int PieceTypeOnSq(const Square square) const;
    Square KingSq(const Color color) const;
    Square EnPassantSq() const;
    bool IsInCheck() const;
    bool LeavesKingInCheck() const;
    void TryMarkingIrreversible(void);
    Bitboard AttacksTo(const Square sq) const;
    Bitboard AllStraightMovers() const;
    Bitboard AllDiagMovers() const;
    Bitboard MapPieceType(const int pieceType) const;
    Bitboard AttacksFrom(const Square sq) const;
    bool MoveGivesCheck(const Move move);
    bool IsWhiteShortCastleLegal();
    bool IsWhiteLongCastleLegal();
    bool IsBlackShortCastleLegal();
    bool IsBlackLongCastleLegal();
};

class Parameters {
public:
    void Init();
    int pawnSupport[2][64];
    int pst[2][6][64];
#ifdef USE_TUNING
    double TryChangeMgPst(Position* pos, int piece, Square sq, int delta, double baselineLoss);
    double TryChangeEgPst(Position* pos, int piece, Square sq, int delta, double baselineLoss);
    void PrintPst(int piece);
    double TuneSingleSquare(Position* pos, int piece, Square s, double currentFit);
#endif
};

extern Parameters Params;

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
    Move GetMove();
    void ScoreNoisy(Position* pos);
    void ScoreQuiet(Position* pos);
};

bool IsBadCapture(Position* pos, Move move);
int Swap(const Position* pos, const Square fromSquare, const Square toSquare);

void TryInterrupting(void);

int InputAvailable(void);
void OnNewGame(void);

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

#ifdef USE_TUNING

struct Sample {
    std::string epd;  // full EPD/FEN line
    double result;    // 1.0 (1-0), 0.0 (0-1), 0.5 (1/2-1/2)
};

#include <vector>
#include <cstdint>
#include <cinttypes>
#include <string>
#include <sstream>     // std::getline with std::istringstream
#include <fstream>     // std::ifstream

class cTuner {
public:
    std::vector<Sample> dataset;
    void Init(int filter);
    double TexelFit(Position* p);
};

extern cTuner Tuner;

#endif