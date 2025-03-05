#pragma once

typedef unsigned long long Bitboard;

#include <iostream>
#include <algorithm>

enum eMoveType { tNormal, tCastle, tEnPassant, tPawnjump, tPromN, tPromB, tPromR, tPromQ };
enum eCastleFlag { wShortCastle = 1, wLongCastle = 2, bShortCastle = 4, bLongCastle = 8};
enum eMoveFlag { moveQuiet, moveHash, moveNoisy};

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

class Parameters {
private:
	void InitPst();
public:
	void Init();
	int mgPst[2][6][64];
	int egPst[2][6][64];
};

extern Parameters Params;

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
	void MovePiece(const Color color, const int typeOfPiece, const Square fromSquare, Square toSquare);
	void AddPiece(const Color color, const int typeOfPiece, const Square square);
	void TakePiece(const Color color, const int typeOfPiece, const Square square);
	void ChangePiece(const int oldType, const int newType, const Color color, const Square square);
	Bitboard CalculateHashKey();
	
	bool IsDrawBy50MoveRule();
	bool IsDrawByRepetition();
	bool IsDrawByInsufficientMaterial();
public:
	Bitboard boardHash;
	bool SquareIsAttacked(const Square sq, Color color);
	void Set(const std::string str);
	void DoMove(const int move, const int ply);
	void DoNull(const int ply);
	void UndoMove(const int move, const int ply);
	void UndoNull(const int ply);
	bool IsDraw();
	bool CanTryNullMove(void);
	Color GetSideToMove();
	int GetPiece(const Square square);
	int Count(const Color color, const int type);
    int CountAllPawns();
    int CountMinors(const Color color);
    int CountMajors(const Color color);
	Bitboard Map(const Color color, const int piece);
	Bitboard Map(const Color color);
	Bitboard Occupied();
	Bitboard Empty();
	Bitboard MapDiagonalMovers(const Color color);
	Bitboard MapStraightMovers(const Color color);
	int PieceTypeOnSq(const Square square);
	Square KingSq(const Color color);
	Square EnPassantSq();
	bool IsInCheck();
	bool LeavesKingInCheck();
	void TryMarkingIrreversible(void);
	bool IsEmpty(const Square sq);
	bool WhiteCanCastleShort();
	bool BlackCanCastleShort();
	bool WhiteCanCastleLong();
	bool BlackCanCastleLong();

	Bitboard AttacksTo(const Square sq);
	Bitboard AllStraightMovers();
	Bitboard AllDiagMovers();
	Bitboard MapPieceType(const int pieceType);
};

// state

class EngineState {
public:
	bool isStopping;
	bool isPondering;
    void Init();
};

extern EngineState State;

// list

class MoveList {
private:
	int moves[MovesLimit];
	int values[MovesLimit];
	int ind;
	int get;
	void AddPromotions(MoveList* list, Square fromSquare, Square toSquare);
	void SwapMoves(const int i, const int j);
public:
	void Clear();
	void AddMove(Square fromSquare, Square toSquare, int flag);
	int GetInd();
	int GetMove();
	bool HasMore();
	void ScoreMoves(Position *pos, int ply, int ttMove);
};

// move generation

void FillQuietList(Position *pos, MoveList *list);
void FillNoisyList(Position *pos, MoveList *list);
void FillCompleteList(Position *pos, MoveList *list);

bool IsBadCapture(Position *pos, int move);
int Swap(Position *pos, Square fromSquare, Square toSquare);

int Clip(int v, int l);

void TryInterrupting(void);
void DisplayPv(int score);

int InputAvailable(void);
void OnNewGame(void);

// diagnostics

void PrintBoard(Position *pos);
Bitboard Perft(Position* pos, int ply, int depth, bool isNoisy);

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