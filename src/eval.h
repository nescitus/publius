#pragma once

#include <cstdint>

// Define the eval hash entry
struct EvalTTEntry {
    Bitboard key;
    int val;
};

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
void EvalPawn(Position* pos, EvalData* e, Color color);
void EvalKnight(Position* pos, EvalData* e, Color color);
void EvalBishop(Position* pos, EvalData* e, Color color);
void EvalRook(Position* pos, EvalData* e, Color color);
void EvalQueen(Position* pos, EvalData* e, Color color);
void EvalKing(Position* pos, EvalData* e, Color color);
void EvalKingAttacks(EvalData* e, Color color);
int GetDrawMul(Position* pos, const Color strong, const Color weak);
int Interpolate(EvalData* e);

const int mgPieceValue[6] = { 93, 402, 407, 595, 1250, 0 };
const int egPieceValue[6] = { 104, 345, 375, 651, 1240, 0 };

// Game phase is used to interpolate between middlegame
// and endgame scores. See Interpolate() for details.
const int phaseTable[6]   = {   0,   1,   1,   2,   4,  0 };
const int MaxGamePhase = 24;

const int passedBonusMg[2][8] = { { 0, 10, 10, 30, 50, 80, 120, 0},
                                  { 0, 120, 80, 50, 30, 10, 10, 0} };

const int passedBonusEg[2][8] = { { 0, 11, 11, 37, 62, 107, 165, 0 },
                                  { 0, 165, 107, 62, 37, 11, 11, 0 } };

const int knightMobMg[9] = { -16, -12,  -8,  -4,   0,   4,   8,  12,  16 };
const int knightMobEg[9] = { -16, -12,  -8,  -4,   0,   4,   8,  12,  16 };
const int bishMobMg[15]  = { -30, -25, -20, -15, -10,  -5,   0,   5,  10,  15,  20,  25,  30,  35,  40};
const int bishMobEg[15]  = { -30, -25, -20, -15, -10,  -5,   0,   5,  10,  15,  20,  25,  30,  35,  40 };
const int rookMobMg[15] =  { -16, -13, -10,  -8,  -6,  -4,  -2,   0,   2,   4,   6,   8,  10,  12,  14 };
const int rookMobEg [15] = { -28, -24, -20, -16, -12,  -8,  -4,   0,   4,   8,  12,  16,  20,  24,  28 };
const int queenMobMg[28] = { -13, -12, -11, -10,  -9,  -8,  -7,  -6 , -5,  -4,  -3,  -2,  -1,   0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14 };
const int queenMobEg[28] = { -28, -24, -22, -20, -18, -16, -14, -12 ,-10,  -8,  -6,  -4,  -2,   0,   2,   4,   6,   8,  10,  12,  14,  16,  18,  20,  22,  24,  26,  28 };

const int tempoMg = 5;
const int tempoEg = 10;
const int bishPairMg = 40;
const int bishPairEg = 60;
const int doubledPawnMg = -9;
const int doubledPawnEg = -9;
const int isolPawnMg = -10;
const int isolPawnEg = -18;
const int rookClosedMg = -6;
const int rookClosedEg = -6;
const int rookHalfMg = 6;
const int rookHalfEg = 6;
const int rookOpenMg = 12;
const int rookOpenEg = 12;
const int rook7thMg = 12;
const int rook7thEg = 30;
const int kingPseudoShield = 8;
const int kingOpenFilePenalty = -8;
const int kingNearOpenPenalty = -6;

const int p_support[64] = {
  0,   0,   0,   0,   0,   0,   0,   0,
  0,   1,   0,  -2,   0,   1,   2,   1,
  0,   0,   0,   2,   2,   0,   1,   1,
  0,   0,   2,   6,   6,   2,   0,   0,
  0,   1,   3,   7,   7,   3,   1,   0,
  3,   5,   5,   6,   6,   5,   4,   3,
  4,   5,   6,   7,   7,   6,   5,   4,
  0,   0,   0,   0,   0,   0,   0,   0,
};

const int mgPawnPst[64] = {
    //A1                                H1
      0,   0,   0,   0,   0,   0,   0,   0,
     -7, -10, -15, -10, -15,   9,   2, -19,
     -7, -12, -11,   0,   0,   6,  -1, -15,
     -3,  -2,   0,   5,  11,   6,  -5, -10,
      1,   6,   3,  13,  20,  12,   2, -15,
     12,  17,  28,  21,  31,  39,  17,  -9,
     31,  41,  41,  25,  27,  37,  10,  11,
      0,   0,   0,   0,   0,   0,   0,   0,
      //A8                                H8
};

const int egPawnPst[64] = {
  0,   0,   0,   0,   0,   0,   0,   0,
  1,   2,  16,  16,  16,  16,   2,   1,
  1,  -1,   3,   2,   2,   3,  -1,   1,
  5,   5,  -3, -11, -11,  -3,   5,   5,
 19,   8,   3, -10, -10,   3,   8,  19,
 40,  38,  28,  11,  11,  28,  38,  40,
 56,  69,  49,  38,  38,  49,  69,  56,
  0,   0,   0,   0,   0,   0,   0,   0,
};

const int mgKnightPst[64] = {
-34,  -7, -25,  -1,  -2,  -1,  -8, -32,
-25, -21,  -8,   6,   5,   0,  -8,  -3,
-17,   0,   3,  16,  19,  11,   6,  -1,
-11,  -4,  13,  14,  23,  12,  16,  -6,
-16,   9,  15,  24,  18,  31,  10,  14,
-14, -10,   7,  25,  27,  54,  23,  -5,
-23, -20,  35,  20,  39,  33, -12,  -8,
-133,-17,  -7, -19,   7,  -4, -36,-124,
};

const int egKnightPst[64] = {
-44, -33,  -8,  -6,  -6,  -8, -33, -44,
-14,   5,  -2,   6,   6,  -2,   5, -14,
-22,   7,  11,  29,  29,  11,   7, -22,
 12,  26,  37,  37,  37,  37,  26,  12,
 19,  35,  33,  45,  45,  33,  35,  19,
  0,  21 , 28,  22,  22,  28,  21,   0,
-16,   1,  -3,  30,  30,  -3,   1, -16,
-68, -20,  -6,   3,   3,  -6, -20, -68,
};

const int mgBishopPst[64] = {
  4,  -8,  -7,  -9, -10,  -4, -29,   2,
  4,   6,  -1,  -7,  -4,  -1,  16,   3,
 -5,  -6,  -4,  -5,  -3,   2,   5,   7,
-16,   3,  -2,   6,   6,  -3,   3,  -8,
-22,   0,  -5,  22,   7,  -5,  -1, -20,
 -9,   2,  16,   2,  20,  28,  -6,   8,
-19,  -9, -16,  -9, -12,  10,  -2,  -1,
-25, -25,  -7, -18, -19,  -3,  -3, -12,
};

const int egBishopPst[64] = {
 -9,  -3,  -8,  -3,  -3,  -8,  -3,  -9,
-23, -21, -11,  -8,  -8, -11, -21, -23,
 -1,   1,   0,   8,   8,   0,   1,  -1,
 -2,   6,  12,   8,   8,  12,   6,  -2,
 11,  26,  12,  10,  10,  12,  26,  11,
 14,  19,   3,  -1,  -1,   3,  19,  14,
  2,   8,  11,   6,   6,  11,   8,   2,
  0,   6,  -5,  12,  12,  -5,   6,   0,
};

const int mgRookPst[64] = {
  2,   2,   4,   7,   8,   9,  -5,   0,
-24, -10, -14,  -7,  -8, -10,   4, -21,
-23, -14, -10,  -7,  -7,  -4,  -2, -21,
-16, -10, -15,  -9, -17, -10,   6, -15,
-12,  -1,  -3,   9,  -1,   8,   4,  -8,
  4,  15,  -5,  11,   5,  13,  17,  12,
-10, -13,   6,   6,  12,  13,  -4,  -1,
 17,   6, -11,   5,  14,   0,  16,  20,
};

const int egRookPst[64] = {
-23, -15, -15, -20, -20, -15, -15, -23,
-26, -20, -19, -21, -21, -19, -20, -26,
-10,  -5, -18, -19, -19, -18,  -5, -10,
 -5,  -2,  -1,   3,   3,  -1,  -2,  -5,
 10,  10,   7,  11,  11,   7,  10,  10,
  7,   2,  18,   7,   7,  18,   2,   7,
  0,   2,  -8,   3,   3,  -8,   2,   0,
 16,  20,  29,  21,  21,  29,  20,  16,
};

const int mgQueenPst[64] = {
  8,  -4,   3,  12,  11,  -4, -22,  -7,
 -6,   4,  10,   7,  10,   9,  10, -12,
-18,   4,   5,   3,   5,   4,  13, -11,
 -3, -10,  -5, -12,   0,  -2,   5,   2,
-20, -13, -16, -21,  -9, -10,   4,  -6,
-18, -18, -17, -20,   8,  12,  22,   6,
-27, -36, -24,  -7, -11,  10, -12,  24,
-27, -16,  -7,   0,  -2,   9,  15,  18,
};

const int egQueenPst[64] = {
-45, -34, -47, -28, -28, -47, -34, -45,
-25, -21, -47, -21, -21, -47, -21, -25,
 -8,  -6,  -1, -13, -13,  -1,  -6,  -8,
 -8,  19,   4,  21,  21,   4,  19,  -8,
  1,  19,  -8,  34,  34,  -8,  19,   1,
-16, -11,   3,   5,   5,   3, -11, -16,
 12, -12,  11,  -9,  -9,  11, -12,  12,
-12,   2,   5,  -1,  -1,   5,   2, -12,
};

const int mgKingPst[64] = {
  9,  33,  13, -27,  -7, -19,  18,  15,
  0,   5, -13, -44, -28, -26,  17,   5,
-19,  15,   5, -10, -13,   0,  14,  -8,
-10,  27,  17,  -9,   5,   2,  13, -14,
  4,   2,  18,  20,  18,  16,  -2, -20,
  1,  23,  24,  13,  14,  18,  19, -24,
 -2,  10,  13,   0,   8,  18,   3,  -9,
-29,  -9,  -3,  -6,   1,  -8, -26, -16,
};

const int egKingPst[64] = {
-68, -18,  -6, -26, -26,  -6, -18, -68,
-14,  -1,  22,  22,  22,  22,  -1, -14,
-13,   6,  25,  38,  38,  25,   6, -13,
-13,  22,  39,  49,  49,  39,  22, -13,
  8,  37,  44,  39,  39,  44,  37,   8,
  3,  39,  45,  28,  28,  45,  39,   3,
-39,  27,  15,  13,  13,  15,  27, -39,
-82, -30, -21, -33, -33, -21, -30, -82,
};