#pragma once

// info depth 19 time 55657 nodes 54190536 nps 973651 score cp 24 pv e2e4 e7e5 b1c3 b8c6 g1f3 f8c5 a2a3 c5b6 f1c4 g8f6 e1g1 e8g8 d2d3 d7d6 c3a4 d6d5 a4b6 a7b6 e4d5 f6d5

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
	void MovePiece(const Color color, const int hunter, const Square fromSquare, Square toSquare);
	void MovePieceNoHash(const Color color, const int hunter, const Square fromSquare, Square toSquare);
	void AddPiece(const Color color, const int typeOfPiece, const Square square);
	void TakePiece(const Color color, const int typeOfPiece, const Square square);
	void ChangePiece(const int oldType, const int newType, const Color color, const Square square);
	void SetEnPassantSquare(const Color color, Square toSquare);
	Bitboard CalculateHashKey();
	
	bool IsDrawBy50MoveRule() const;
	bool IsDrawByRepetition() const;
	bool IsDrawByInsufficientMaterial() const;
public:
	Bitboard boardHash;
	bool SquareIsAttacked(const Square sq, Color color) const;
	void Set(const std::string str);
	void DoMove(const int move, const int ply);
	void DoNull(const int ply);
	void UndoMove(const int move, const int ply);
	void UndoNull(const int ply);
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
	bool IsEmpty(const Square sq) const;
	bool WhiteCanCastleShort() const;
	bool BlackCanCastleShort() const;
	bool WhiteCanCastleLong() const;
	bool BlackCanCastleLong() const;

	Bitboard AttacksTo(const Square sq) const;
	Bitboard AllStraightMovers() const;
	Bitboard AllDiagMovers() const;
	Bitboard MapPieceType(const int pieceType) const;

	void UpdateCastlingRights(const Square fromSquare, const Square toSquare);
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