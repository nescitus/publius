// REGEX to count all the lines under MSVC 13: ^(?([^\r\n])\s)*[^\s+?/]+[^\n]*$
// 2428 lines

#pragma once

typedef unsigned long long Bitboard;

#include <iostream>
#include <algorithm>

// color

enum Color { White, Black, colorNone };
inline Color operator~(Color c) { return Color(c ^ Black); }          // switch color
inline Color operator++(Color& d) { return d = Color(int(d) + 1); }   // step through colors

// square

enum Square {
	A1, B1, C1, D1, E1, F1, G1, H1,
	A2, B2, C2, D2, E2, F2, G2, H2,
	A3, B3, C3, D3, E3, F3, G3, H3,
	A4, B4, C4, D4, E4, F4, G4, H4,
	A5, B5, C5, D5, E5, F5, G5, H5,
	A6, B6, C6, D6, E6, F6, G6, H6,
	A7, B7, C7, D7, E7, F7, G7, H7,
	A8, B8, C8, D8, E8, F8, G8, H8
};

static constexpr Square sqNone = (Square)64;

inline Square operator++(Square& d) { return d = Square(int(d) + 1); }            // step through squares
inline Square operator+(Square d1, int d2) { return Square(int(d1) + int(d2)); }  // add vector to square
inline Square operator-(Square d1, int d2) { return Square(int(d1) - int(d2)); }  // substract vector from square
inline Square operator^(Square d1, int d2) { return Square(int(d1) ^ d2); }       // needed for en passant

// limits

constexpr int Infinity = 32767;
constexpr int MateScore = 32000;
constexpr int EvalLimit = 29999;
constexpr int IntLimit = 2147483646;
constexpr int HistLimit = (1 << 15);
constexpr int PlyLimit = 64;
constexpr int MovesLimit = 256;

enum eFile { fileA, fileB, fileC, fileD, fileE, fileF, fileG, fileH };
enum eRank { rank1, rank2, rank3, rank4, rank5, rank6, rank7, rank8 };
enum eMoveType { tNormal, tCastle, tEnPassant, tPawnjump, tPromN, tPromB, tPromR, tPromQ };
enum eCastleFlag { wShortCastle = 1, wLongCastle = 2, bShortCastle = 4, bLongCastle = 8};
enum eHashEntry { None, lowerBound, upperBound, exactEntry };
enum eMoveFlag { moveQuiet, moveHash, moveNoisy};

static constexpr auto sideRandom = ~((Bitboard)0);

static constexpr auto startFen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -";
static constexpr auto kiwipeteFen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -";

// stack

typedef struct {
	int move;
	int prey;
	int castleFlags;
	Square enPassantSq;
	int reversibleMoves;
	Bitboard boardHash;
} UndoStack;

static const int stackSize = 2048;
extern UndoStack undoStack[stackSize];

// position

class Position {
private:
	Color sideToMove;
	Bitboard pieceBitboard[2][6];
	Square enPassantSq;
	Square kingSq[2];
	int pieceCount[2][6];
	int pieceLocation[64];
	int reversibleMoves;
	int repetitionIndex;
	Bitboard repetitionList[256];

	void SwitchSide();
	void ClearEnPassant();
	void MovePiece(Color color, int typeOfPiece, Square fromSquare, Square toSquare);
	void AddPiece(Color color, int typeOfPiece, Square square);
	void TakePiece(Color color, int typeOfPiece, Square square);
	void ChangePiece(int oldType, int newType, Color color, Square square);
	bool IsDrawBy50MoveRule();
	bool IsDrawByRepetition();
	bool IsDrawByInsufficientMaterial();
public:
	int castleFlags;
	Bitboard boardHash;
	int SquareIsAttacked(Square s, Color c);
	void Clear();
	Bitboard CalculateHashKey();
	void Set(std::string str);
	void DoMove(int move, int ply);
	void DoNull(int ply);
	void UndoMove(int move, int ply);
	void UndoNull(int ply);
	bool IsDraw();
	bool IsNullMoveOk(void);

	Color GetSide();
	int GetPiece(Square square);
	int GetCount(Color color, int type);
    int GetAllPawnsCount();
    int GetMinorCount(Color color);
    int GetMajorCount(Color color);
	Bitboard Map(Color color, int piece);
	Bitboard Map(Color color);
	Bitboard Occupied();
	Bitboard Empty();
	Bitboard MapDiagonalMovers(Color color);
	Bitboard MapStraightMovers(Color color);
	int PieceTypeOnSq(Square square);
	Square KingSq(Color color);
	Square EnPassantSq();
	bool IsInCheck();
	bool LeavesKingInCheck();
	void TryMarkingIrreversible(void);

	bool IsEmpty(Square sq);
};

// state

class EngineState {
public:
	bool isStopping;
	bool isPondering;
    void Init();
};

extern EngineState State;

// evalDdata

struct evalData {
public:
	int mg[2];
	int eg[2];
	int phase; // game phase (24 for starting position)
};

// list

class MoveList {
private:
	int moves[MovesLimit];
	int values[MovesLimit];
	int ind;
	int get;
public:
	void Clear();
	void AddMove(Square fromSquare, Square toSquare, int flag);
	int GetInd();
	int GetMove();
	bool HasMore();
	void SwapMoves(int i, int j);
	void ScoreMoves(Position *pos, int ply, int ttMove);
};

// move generation

void AddPromotions(MoveList *list, Square fromSquare, Square toSquare);
void FillQuietList(Position *pos, MoveList *list);
void FillNoisyList(Position *pos, MoveList *list);
void FillCompleteList(Position *pos, MoveList *list);

int Clip(int v, int l);

void TryInterrupting(void);
void DisplayPv(int score);

int Evaluate(Position *pos, evalData * e);

void InitLmr();
void InitPst(void);
int InputAvailable(void);
Bitboard Random64(void);
void ResetEngine(void);

// diagnostics

void PrintBoard(Position *pos);
Bitboard Perft(Position* pos, int ply, int depth, bool isNoisy);

extern int pvLine[PlyLimit + 2][PlyLimit + 2];
extern int pvSize[PlyLimit + 2];

// TODO: params class
extern int mgTable[2][6][64];
extern int egTable[2][6][64];

extern const int bitTable[64];
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