// Publius - Didactic public domain bitboard chess engine by Pawel Koziol

#pragma once

#include <cstdint>

// Define the pawn hash entry

struct sPawnHashEntry {
    Bitboard key;
    int val[2];
};

// Define the eval hash entry
struct EvalTTEntry {
    Bitboard key;
    int val;
};

const int PAWN_HASH_SIZE = 16384;

extern sPawnHashEntry PawnTT[PAWN_HASH_SIZE];

class EvalHashTable {
public:
    EvalHashTable(size_t size); // constructor
    ~EvalHashTable(); // destructor
    void Save(Bitboard key, int val);
    bool Retrieve(Bitboard key, int* score) const;

private:
    size_t Hash(Bitboard key) const;
    EvalTTEntry* EvalTT;    // Dynamically allocated array
    const size_t tableSize; // Size of the hash table (must be a power of two)
};

extern EvalHashTable EvalHash;

int Evaluate(Position* pos, EvalData* e);
void EvalBasic(EvalData* e, const Color color, const int piece, const int sq);
void EvalPawnStructure(const Position* pos, EvalData* e);
void EvalPawn(const Position* pos, EvalData* e, Color color);
void EvalKnight(const Position* pos, EvalData* e, Color color);
void EvalBishop(const Position* pos, EvalData* e, Color color);
void EvalRook(const Position* pos, EvalData* e, Color color);
void EvalQueen(const Position* pos, EvalData* e, Color color);
void EvalKing(const Position* pos, EvalData* e, Color color);
void EvalKingAttacks(EvalData* e, Color color);
void EvalPasser(const Position* pos, EvalData* e, Color color);
void EvalPressure(Position* pos, EvalData* e, Color side);
int GetDrawMul(Position* pos, const Color strong, const Color weak);
int Interpolate(EvalData* e);

const int pieceValue[6] = {
    S(93, 104), S(402, 345), S(407, 375), S(589, 645), S(1250, 1240), S(0, 0) 
};

const int pressureBonus[7] = { 
    S(1, 2), S(10, 14), S(10, 14), S(12, 17), S(20,24), 0, 0 
};

// Game phase is used to interpolate between middlegame
// and endgame scores. See Interpolate() for details.
const int phaseTable[6] = { 0,   1,   1,   2,   4,  0 };
const int MaxGamePhase = 24;

const int passedBonus[2][8] = {
    { S(0, 11), S(10, 11), S(10, 11), S(30, 37), S(50, 62), S(80, 107), S(120, 165), S(0, 0) },
    { S(0, 165), S(120, 165), S(80, 107), S(50, 62), S(30, 37), S(10, 11), S(10, 11), S(0, 0) }
};

const int knightMob[9] = { 
    S(-28, -28), S(-6, -8), S(-3,-10), S(-2,6), S(16,0), S(17,5), S(17, 8), S(20, 9), S(25, 8)
};

const int bishMob[15] = {
    S(-30, -30), S(-29, -37), S(-23, -38), S(-11, -21), S(-5, -20),
    S(2,   -8),  S(8,    6),  S(12,  12),  S(20,  14),  S(19,  15),
    S(23,  25),  S(28,  18),  S(35,  24),  S(44,  20),  S(40,  25)
}; 

const int rookMob[15] = {
    S(-14, -28), S(-12, -24), S(-13, -25), S(-11, -19), S(-8, -11),
    S(-6,  -4),  S(-8,   3),  S(-3,  10),  S(2,  10),  S(2, 16),
    S(7,  18),  S(14,  21),  S(17,  28),  S(17,  34),  S(10, 37)
};

const int queenMob[28] = {
    S(-14, -28), S(-13, -26), S(-12, -24), S(-30, -30), S(-28, -43),
    S(-17, -26), S(-11, -23), S(-2,  -28), S(-6,  -34), S(  0, -21),
    S(  3, -24), S(  1, -22), S(  1,  -7), S(  1,  -2), S(  4,  -9),
    S(  6,  -1), S(  5,   3), S(  0,  19), S(  3,  25), S(  2,  19),
    S(  7,  20), S(  3,  31), S(  6,  26), S(  2,  29), S( 15,  21),
    S( 21,  13), S( 12,  17), S( 14,  22)
};

const int tempo = S(5, 10);
const int bishPair = S(40, 60);
const int doubledPawn = S(-9,-9);
const int isolPawn = S(-10, -18);
const int isolOpen = S(-9, 0);
const int backwardPawn = S(-2, -1);
const int backwardOpen = S(-6, 0);
const int rookHalf = S(12, 12);
const int rookOpen = S(18, 18);
const int rook7th =  S(12, 30);
const int kingPseudoShield = S(8, 0);
const int kingOpenFilePenalty = S(-8, 0);
const int kingNearOpenPenalty = S(-6, 0);
const int minorOnMinor = S(15, 15);
const int trappedRook = S(-25, -25);

const int p_support[64] = {
  0,   0,   0,   0,   0,   0,   0,   0,
  3,   3,   1,  -1,   2,   5,   6,   4,
  3,   0,   2,   3,   4,   2,   4,   6,
  1,   1,   6,  17,  10,   5,   1,   1,
  3,   4,   8,  14,  15,   9,   4,   3,
  7,   9,  10,  12,  12,  10,   9,   7,
  9,  10,  12,  14,  14,  12,  10,   9,
  0,   0,   0,   0,   0,   0,   0,   0,
};

const int pawnPst[64] = {
  // A1
  S(  0,   0), S(  0,   0), S(  0,   0), S(  0,   0), S(  0,   0), S(  0,   0), S(  0,   0), S(  0,   0),
  S( -7,   1), S(-10,   2), S(-15,  16), S(-10,  16), S(-15,  16), S(  9,  16), S(  2,   2), S(-19,   1),
  S( -7,   1), S(-12,  -1), S(-11,   3), S(  0,   2), S(  0,   2), S(  6,   3), S( -1,  -1), S(-15,   1),
  S( -3,   5), S( -2,   5), S(  0,  -3), S(  5, -11), S( 11, -11), S(  6,  -3), S( -5,   5), S(-10,   5),
  S(  1,  19), S(  6,   8), S(  3,   3), S( 13, -10), S( 20, -10), S( 12,   3), S(  2,   8), S(-15,  19),
  S( 12,  40), S( 17,  38), S( 28,  28), S( 21,  11), S( 31,  11), S( 39,  28), S( 17,  38), S( -9,  40),
  S( 31,  56), S( 41,  69), S( 41,  49), S( 25,  38), S( 27,  38), S( 37,  49), S( 10,  69), S( 11,  56),
  S(  0,   0), S(  0,   0), S(  0,   0), S(  0,   0), S(  0,   0), S(  0,   0), S(  0,   0), S(  0,   0),
};

const int knightPst[64] = {
  S(-34, -44), S( -7, -33), S(-25,  -8), S( -1,  -6), S( -2,  -6), S( -1,  -8), S( -8, -33), S(-32, -44),
  S(-25, -14), S(-21,   5), S( -8,  -2), S(  6,   6), S(  5,   6), S(  0,  -2), S( -8,   5), S( -3, -14),
  S(-17, -22), S(  0,   7), S(  3,  11), S( 16,  29), S( 19,  29), S( 11,  11), S(  6,   7), S( -1, -22),
  S(-11,  12), S( -4,  26), S( 13,  36), S( 14,  37), S( 23,  37), S( 12,  36), S( 16,  26), S( -6,  12),
  S(-16,  19), S(  9,  35), S( 15,  33), S( 24,  43), S( 18,  43), S( 31,  33), S( 10,  35), S( 14,  19),
  S(-14,   0), S(-10,  21), S(  7,  27), S( 25,  21), S( 27,  21), S( 54,  27), S( 23,  21), S( -5,   0),
  S(-23, -16), S(-20,   1), S( 35,  -3), S( 20,  30), S( 39,  30), S( 33,  -3), S(-12,   1), S( -8, -16),
  S(-133,-68), S(-17, -20), S( -7,  -6), S(-19,   3), S(  7,   3), S( -4,  -6), S(-36, -20), S(-124,-68),
};

const int bishopPst[64] = {
  S(  4, -9), S( -8, -3), S( -7, -8), S( -9, -3), S(-10, -3), S( -4, -8), S(-29, -3), S(  2, -9),
  S(  4,-23), S(  6,-21), S( -1,-11), S( -7, -8), S( -4, -8), S( -1,-11), S( 16,-21), S(  3,-23),
  S( -5, -1), S( -6,  1), S( -4,  0), S( -5,  8), S( -3,  8), S(  2,  0), S(  5,  1), S(  7, -1),
  S(-16, -2), S(  3,  6), S( -2, 12), S(  6,  8), S(  6,  8), S( -3, 12), S(  3,  6), S( -8, -2),
  S(-22, 11), S(  0, 26), S( -5, 12), S( 22, 10), S(  7, 10), S( -5, 12), S( -1, 26), S(-20, 11),
  S( -9, 14), S(  2, 19), S( 16,  3), S(  2, -1), S( 20, -1), S( 28,  3), S( -6, 19), S(  8, 14),
  S(-19,  2), S( -9,  8), S(-16, 11), S( -9,  6), S(-12,  6), S( 10, 11), S( -2,  8), S( -1,  2),
  S(-25,  0), S(-25,  6), S( -7, -5), S(-18, 12), S(-19, 12), S( -3, -5), S( -3,  6), S(-12,  0),
};

const int rookPst[64] = {
  S(  2, -23), S(  2, -15), S(  4, -15), S(  7, -20), S(  8, -20), S(  9, -15), S( -5, -15), S(  0, -23),
  S(-24, -26), S(-10, -20), S(-14, -19), S( -7, -21), S( -8, -21), S(-10, -19), S(  4, -20), S(-21, -26),
  S(-23, -10), S(-14,  -5), S(-10, -18), S( -7, -19), S( -7, -19), S( -4, -18), S( -2,  -5), S(-21, -10),
  S(-16,  -5), S(-10,  -2), S(-15,  -1), S( -9,   3), S(-17,   3), S(-10,  -1), S(  6,  -2), S(-15,  -5),
  S(-12,  10), S( -1,  10), S( -3,   7), S(  9,  11), S( -1,  11), S(  8,   7), S(  4,  10), S( -8,  10),
  S(  4,   7), S( 15,   2), S( -5,  18), S( 11,   7), S(  5,   7), S( 13,  18), S( 17,   2), S( 12,   7),
  S(-10,   0), S(-13,   2), S(  6,  -8), S(  6,   3), S( 12,   3), S( 13,  -8), S( -4,   2), S( -1,   0),
  S( 17,  16), S(  6,  20), S(-11,  29), S(  5,  21), S( 14,  21), S(  0,  29), S( 16,  20), S( 20,  16),
};

const int queenPst[64] = {
  S(  8, -45), S( -4, -34), S(  3, -47), S( 12, -28), S( 11, -28), S( -4, -47), S(-22, -34), S( -7, -45),
  S( -6, -25), S(  4, -21), S( 10, -47), S(  7, -21), S( 10, -21), S(  9, -47), S( 10, -21), S(-12, -25),
  S(-18,  -8), S(  4,  -6), S(  5,  -1), S(  3, -13), S(  5, -13), S(  4,  -1), S( 13,  -6), S(-11,  -8),
  S( -3,  -8), S(-10,  19), S( -5,   4), S(-12,  21), S(  0,  21), S( -2,   4), S(  5,  19), S(  2,  -8),
  S(-20,   1), S(-13,  19), S(-16,  -8), S(-21,  34), S( -9,  34), S(-10,  -8), S(  4,  19), S( -6,   1),
  S(-18, -16), S(-18, -11), S(-17,   3), S(-20,   5), S(  8,   5), S( 12,   3), S( 22, -11), S(  6, -16),
  S(-27,  12), S(-36, -12), S(-24,  11), S( -7,  -9), S(-11,  -9), S( 10,  11), S(-12, -12), S( 24,  12),
  S(-27, -12), S(-16,   2), S( -7,   5), S(  0,  -1), S( -2,  -1), S(  9,   5), S( 15,   2), S( 18, -12),
};

const int kingPst[64] = {
  S(  9, -68), S( 33, -18), S( 13,  -6), S(-27, -26), S( -7, -26), S(-19,  -6), S( 18, -18), S( 15, -68),
  S(  0, -14), S(  5,  -1), S(-13,  22), S(-44,  22), S(-28,  22), S(-26,  22), S( 17,  -1), S(  5, -14),
  S(-19, -13), S( 15,   6), S(  5,  25), S(-10,  38), S(-13,  38), S(  0,  25), S( 14,   6), S( -8, -13),
  S(-10, -13), S( 27,  22), S( 17,  39), S( -9,  49), S(  5,  49), S(  2,  39), S( 13,  22), S(-14, -13),
  S(  4,   8), S(  2,  37), S( 18,  44), S( 20,  39), S( 18,  39), S( 16,  44), S( -2,  37), S(-20,   8),
  S(  1,   3), S( 23,  39), S( 24,  45), S( 13,  28), S( 14,  28), S( 18,  45), S( 19,  39), S(-24,   3),
  S( -2, -39), S( 10,  27), S( 13,  15), S(  0,  13), S(  8,  13), S( 18,  15), S(  3,  27), S( -9, -39),
  S(-29, -82), S( -9, -30), S( -3, -21), S( -6, -33), S(  1, -33), S( -8, -21), S(-26, -30), S(-16, -82),
};