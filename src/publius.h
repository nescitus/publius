#pragma once

typedef unsigned long long Bitboard;

#include <iostream>
#include <algorithm>

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

class EvalData {
public:
	int mg[2];
	int eg[2];
	Bitboard enemyKingZone[2];
	int minorAttacks[2];
	int rookAttacks[2];
	int queenAttacks[2];
	int phase; // game phase (24 for starting position)
	void Clear();
	void Add(Color color, int mgVal, int egVal);
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

int Evaluate(Position *pos, EvalData * e);

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