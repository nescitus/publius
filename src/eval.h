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

// 77.213972

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
  S(  0,  0), S(  0,  0), S(  0,  0), S(  0,  0), S(  0,  0), S(  0,  0), S(  0,  0), S(  0,  0),
  S( -6,  0), S(-10,  0), S(-11, 14), S(-14, 14), S(-13, 14), S( 12, 14), S(  6,  0), S(-17,  0),
  S(-11,  0), S(-12,  0), S(-14,  1), S( -4,  0), S( -4,  0), S(  2,  1), S(  2,  0), S(-15,  0),
  S( -7,  3), S( -6,  3), S( -4, -2), S(  4,-13), S( 12,-13), S(  4, -2), S( -8,  3), S(-14,  3),
  S( -1, 18), S(  3, 10), S(  7,  2), S( 13,-11), S( 24,-11), S( 16,  2), S(  6, 10), S(-11, 18),
  S( 15, 42), S( 15, 40), S( 24, 26), S( 20,  9), S( 27,  9), S( 43, 26), S( 21, 40), S( -5, 42),
  S( 30, 54), S( 37, 67), S( 37, 47), S( 26, 36), S( 23, 36), S( 33, 47), S(  6, 67), S(  7, 54),
  S(  0,  0), S(  0,  0), S(  0,  0), S(  0,  0), S(  0,  0), S(  0,  0), S(  0,  0), S(  0,  0)};

const int knightPst[64] = {
  S(-38,-42), S(-11,-35), S(-29,-10), S( -5, -8), S( -6, -8), S( -5,-10), S(-12,-35), S(-36,-42),
  S(-29,-15), S(-25,  3), S(-12,  0), S(  2,  4), S(  1,  4), S( -2,  0), S(-12,  3), S( -7,-15),
  S(-21,-20), S( -4,  8), S(  7, 13), S( 12, 31), S( 21, 31), S( 13, 13), S( 10,  8), S( -5,-20),
  S(-10, 10), S( -3, 28), S( 17, 38), S( 18, 39), S( 27, 39), S( 16, 38), S( 20, 28), S( -4, 10),
  S(-15, 17), S( 10, 35), S( 19, 35), S( 28, 45), S( 22, 45), S( 35, 35), S( 14, 35), S( 18, 17),
  S(-13,  2), S( -6, 23), S( 11, 29), S( 29, 23), S( 31, 23), S( 58, 29), S( 27, 23), S( -1,  2),
  S(-23,-14), S(-17,  3), S( 36, -1), S( 24, 28), S( 37, 28), S( 37, -1), S( -9,  3), S( -4,-14),
  S(-137,-70), S(-21,-22), S( -7, -6), S(-20,  4), S(  7,  4), S( -8, -6), S(-36,-22), S(-126,-70)
};

const int bishopPst[64] = {
  S(  1,-11), S( -5, -3), S(-10,-10), S(-12, -5), S(-13, -5), S( -7,-10), S(-26, -3), S( -1,-11),
  S(  3,-22), S(  3,-19), S(  2, -9), S(-10, -6), S( -4, -6), S(  2, -9), S( 19,-19), S(  6,-22),
  S( -3, -1), S( -3,  3), S( -1,  2), S( -2, 10), S(  0, 10), S(  4,  2), S(  4,  3), S(  8, -1),
  S(-17,  0), S(  0,  8), S(  1, 14), S(  9, 10), S(  9, 10), S(  0, 14), S(  1,  8), S( -7,  0),
  S(-23, 12), S(  1, 27), S( -2, 14), S( 25, 12), S( 10, 12), S( -2, 14), S(  2, 27), S(-17, 12),
  S(-12, 13), S(  5, 20), S( 19,  5), S(  5,  1), S( 23,  1), S( 31,  5), S( -3, 20), S( 11, 13),
  S(-22,  0), S(-10, 10), S(-15, 11), S( -8,  8), S( -9,  8), S( 13, 11), S(  1, 10), S(  2,  0),
  S(-28, -2), S(-28,  5), S( -7, -3), S(-21, 10), S(-22, 10), S( -4, -3), S( -1,  5), S(-15, -2)
};

const int rookPst[64] = {
  S( -1,-25), S( -1,-13), S(  6,-15), S( 10,-18), S( 11,-18), S( 10,-15), S( -2,-13), S( -3,-25),
  S(-26,-25), S(-12,-18), S(-12,-18), S(-10,-20), S( -9,-20), S( -8,-18), S(  4,-18), S(-22,-25),
  S(-26,-12), S(-15, -7), S(-13,-18), S( -9,-19), S(-10,-19), S( -6,-18), S(  1, -7), S(-18,-12),
  S(-16, -5), S(-10, -3), S(-12, -1), S( -6,  1), S(-15,  1), S(-13, -1), S(  7, -3), S(-12, -5),
  S(-14,  8), S(  0,  9), S(  0,  8), S(  7,  9), S(  2,  9), S( 11,  8), S(  7,  9), S( -5,  8),
  S(  6,  5), S( 18,  4), S( -2, 18), S( 14,  7), S(  8,  7), S( 14, 18), S( 20,  4), S( 15,  5),
  S(-12, -2), S(-10,  2), S(  9, -6), S(  9,  2), S( 14,  2), S( 16, -6), S( -1,  2), S(  2, -2),
  S( 16, 16), S(  7, 20), S( -8, 31), S(  8, 23), S( 16, 23), S(  3, 31), S( 18, 20), S( 23, 16)
};

const int queenPst[64] = {
  S(  9,-47), S( -7,-36), S(  0,-49), S(  9,-30), S(  8,-30), S( -7,-49), S(-21,-36), S( -8,-47),
  S( -9,-27), S(  1,-23), S( 10,-49), S( 10,-23), S(  7,-23), S( 12,-49), S(  7,-23), S(-15,-27),
  S(-21, -9), S(  1, -8), S(  2, -3), S(  0,-13), S(  3,-13), S(  7, -3), S( 16, -8), S( -8, -9),
  S( -6,-10), S(-12, 18), S( -6,  6), S( -9, 23), S(  2, 23), S(  1,  6), S(  8, 18), S(  5,-10),
  S(-22,  3), S(-16, 17), S(-13, -6), S(-18, 36), S( -6, 36), S( -7, -6), S(  7, 17), S( -3,  3),
  S(-21,-14), S(-17, -9), S(-14,  5), S(-17,  7), S( 11,  7), S( 15,  5), S( 25, -9), S(  9,-14),
  S(-30, 10), S(-36,-10), S(-21, 11), S( -4, -7), S( -8, -7), S( 13, 11), S( -9,-10), S( 27, 10),
  S(-29,-12), S(-19,  0), S( -6,  7), S(  3,  1), S(  1,  1), S( 10,  7), S( 18,  0), S( 17,-12)
};

const int kingPst[64] = {
  S( 10,-66), S( 36,-20), S( 16, -8), S(-30,-26), S(-10,-26), S(-22, -8), S( 22,-20), S( 18,-66),
  S(  3,-12), S(  4,  1), S(-14, 23), S(-47, 23), S(-31, 23), S(-29, 23), S( 14,  1), S(  8,-12),
  S(-20,-15), S( 12,  5), S(  2, 23), S(-13, 36), S(-16, 36), S( -3, 23), S( 11,  5), S(-11,-15),
  S(-12,-13), S( 24, 20), S( 14, 37), S( -9, 48), S(  2, 48), S( -1, 37), S( 10, 20), S(-15,-13),
  S(  1,  6), S( -1, 37), S( 17, 44), S( 19, 41), S( 16, 41), S( 13, 44), S( -5, 37), S(-23,  6),
  S(  0,  5), S( 24, 41), S( 23, 45), S( 15, 30), S( 17, 30), S( 19, 45), S( 22, 41), S(-23,  5),
  S(  1,-37), S( 10, 29), S( 14, 17), S(  2, 15), S(  8, 15), S( 20, 17), S(  5, 29), S( -6,-37),
  S(-28,-80), S( -8,-28), S( -3,-19), S( -5,-31), S(  2,-31), S( -5,-19), S(-24,-28), S(-14,-80)
 };