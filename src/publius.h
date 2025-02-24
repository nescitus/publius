// REGEX to count all the lines under MSVC 13: ^(?([^\r\n])\s)*[^\s+?/]+[^\n]*$
// 2273 lines

#pragma once

typedef unsigned long long Bitboard;

#include <iostream>
#include <algorithm>

// color

enum Color { White, Black, colorNone };
inline Color operator~(Color c) { return Color(c ^ Black); }          // switch color
inline Color operator++(Color& d) { return d = Color(int(d) + 1); }   // step through colors

// piece type

enum PieceType { Pawn, Knight, Bishop, Rook, Queen, King, noPieceType };

Color ColorOfPiece(int piece);                     // given a piece value, return its color
int TypeOfPiece(int piece);                        // given a piece value, return its type
int CreatePiece(Color pieceColor, int pieceType);  // get unique id of a piece (denoting, say, white knight), range 0-11
static const int noPiece = 12;                     // constant just outside this range to denote no piece

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

#define	sqNone (H8 + 1)

inline Square operator++(Square& d) { return d = Square(int(d) + 1); }            // step through squares
inline Square operator+(Square d1, int d2) { return Square(int(d1) + int(d2)); }  // add vector to square
inline Square operator-(Square d1, int d2) { return Square(int(d1) - int(d2)); }  // substract vector from square
inline Square operator^(Square d1, int d2) { return Square(int(d1) ^ d2); }       // needed for en passant

Square MakeSquare(int rank, int file);
int RankOf(Square square);
int FileOf(Square square);
Square InvertSquare(Square square);
Square RelativeSq(Color color, Square square);

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

#define SIDE_RANDOM     (~((Bitboard)0))

#define START_POS       "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -"
#define KIWIPETE        "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -"

// bitboard

static const Bitboard excludeA = 0xfefefefefefefefe;
static const Bitboard excludeH = 0x7f7f7f7f7f7f7f7f;

Bitboard Paint(Square s);
Bitboard Paint(Square s1, Square s2);
Bitboard Paint(Square s1, Square s2, Square s3);

Square FirstOne(Bitboard b);
int PopCnt(Bitboard b);
Square PopFirstBit(Bitboard * b);

Bitboard NorthOf(Bitboard b);
Bitboard SouthOf(Bitboard b);
Bitboard WestOf(Bitboard b);
Bitboard EastOf(Bitboard b);
Bitboard NWOf(Bitboard b);
Bitboard NEOf(Bitboard b);
Bitboard SEOf(Bitboard b);
Bitboard SWOf(Bitboard b);

Bitboard SidesOf(Bitboard b);
Bitboard ForwardOf(Bitboard b, Color c);
Bitboard FrontSpan(Bitboard b, Color c);

Bitboard FillNorth(Bitboard b);
Bitboard FillSouth(Bitboard b);
Bitboard GetWPAttacks(Bitboard b);
Bitboard GetBPAttacks(Bitboard b);

Bitboard FillOcclSouth(Bitboard b, Bitboard o);
Bitboard FillOcclNorth(Bitboard b, Bitboard o);
Bitboard FillOcclEast(Bitboard b, Bitboard o);
Bitboard FillOcclNE(Bitboard b, Bitboard o);
Bitboard FillOcclSE(Bitboard b, Bitboard o);
Bitboard FillOcclWest(Bitboard b, Bitboard o);
Bitboard FillOcclNW(Bitboard b, Bitboard o);
Bitboard FillOcclSW(Bitboard b, Bitboard o);

// bitgen

class MoveGenerator {
private:
	Bitboard pawnAttacks[2][64];
	Bitboard knightAttacks[64];
	Bitboard kingAttacks[64];
    void InitPawnAttacks(Square square, Bitboard b);
    void InitKnightAttacks(Square square, Bitboard b);
    void InitKingAttacks(Square square, Bitboard b);
public:
	void Init(void);
	Bitboard Pawn(Color c, Square s);
	Bitboard Knight(Square s);
	Bitboard Bish(Bitboard o, Square s);
	Bitboard Rook(Bitboard o, Square s);
	Bitboard King(Square s);
};

extern MoveGenerator GenerateMoves;

// mask

class cMask {
private:
    void InitHashKeys();
    void InitRanks();
    void InitPassedMask();
    void InitAdjacentMask();
    void InitSupportMask();
public:
	Bitboard pieceKey[12][64];
	Bitboard castleKey[16];
	Bitboard enPassantKey[8];
	Bitboard rank[8];
    Bitboard file[8];
    Bitboard passed[2][64];
    Bitboard support[2][64];
    Bitboard adjacent[8];
	void Init();
};

extern cMask Mask;

// stack

typedef struct {
	int move;
	int prey;
	int castleFlags;
	Square enPassantSq;
	int reversibleMoves;
	Bitboard boardHash;
} Stack;

static const int stackSize = 2048;
extern Stack undoStack[stackSize];

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

// history

class cHistory {
private:
    int killer[PlyLimit];
	int history[12][64];
	void Trim(void);
public:
	void Clear(void);
	void Update(Position *pos, int mv, int d, int ply);
	int Get(Position *pos, int mv);
    int GetKiller(int ply);
};

extern cHistory History;

// list

class MoveList {
private:
	int moves[MovesLimit];
	int values[MovesLimit];
	int ind;
	int get;
public:
	void Clear();
	void AddMove(Square f, Square t, int flag);
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

// transposition table

typedef struct {
	Bitboard key;
	short move;
	short score;
	unsigned char flags;
	unsigned char depth;
} hashRecord;

class TransTable {
private:
	hashRecord * table;
	int tableSize;
public:
	void Clear(void);
	void Allocate(int mbsize);
	bool Retrieve(Bitboard key, int *move, int *score, int *flag, int alpha, int beta, int depth, int ply);
	void Store(Bitboard key, int move, int score, int flags, int depth, int ply);
	void Exit(void);
};

extern TransTable TT;

int Clip(int v, int l);

// move

int GetTypeOfMove(int mv);
int GetPromotedPiece(int mv);
Square GetFromSquare(int mv);
Square GetToSquare(int mv);
int CreateMove(Square f, Square t, int flag);
bool IsMovePromotion(int mv);
bool IsMoveNoisy(Position *pos, int move);

void TryInterrupting(void);
void DisplayPv(int score);

int Evaluate(Position *pos, evalData * e);
void EvalSinglePiece(Position *pos, evalData * e, Color c, int piece);
int GetDrawMul(Position *pos, Color strong, Color weak);

void InitLmr();
void InitPst(void);
int InputAvailable(void);
void Iterate(Position *pos);
std::string MoveToString(int move);
Bitboard Random64(void);
void ResetEngine(void);

int Widen(Position *pos, int d, int lastScore);
int Search(Position *pos, int ply, int a, int b, int d, bool wasNull);
int Quiesce(Position *pos, int ply, int a, int b);
int GetMoveType(Position *pos, int move, int ttMove);
void ClearPvLine();
void PrintRootInfo(int elapsed, int nps);

int StringToMove(Position *pos, const std::string& moveString);
void Think(Position *pos);
int Timeout(void);
void RefreshPv(int ply, int move);

// diagnostics

void PrintBoard(Position *pos);
int Perft(Position* pos, int ply, int depth, bool isNoisy);

// uci

void UciLoop(void);
bool ParseCommand(std::istringstream& stream, Position *pos);
void OnUciCommand();
void OnPositionCommand(std::istringstream& stream, Position *pos);
void OnGoCommand(std::istringstream& stream, Position *pos);
void OnSetOptionCommand(std::istringstream& stream);
void OnPerftCommand(std::istringstream& stream, Position* p);

std::string ToLower(const std::string& str);
bool IsSameOrLowercase(const std::string& str1, const std::string& str2);

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