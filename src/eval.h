#pragma once

void EvalBasic(EvalData* e, const Color color, const int piece, const int sq);
void EvalPawn(Position* pos, EvalData* e, Color color);
void EvalKnight(Position* pos, EvalData* e, Color color);
void EvalBishop(Position* pos, EvalData* e, Color color);
void EvalRook(Position* pos, EvalData* e, Color color);
void EvalQueen(Position* pos, EvalData* e, Color color);
void EvalKing(Position* pos, EvalData* e, Color color);
void EvalAttacks(EvalData* e, Color color);
int GetDrawMul(Position* pos, Color strong, Color weak);

const int mgPieceValue[6] = {  93, 400, 401, 599, 1250, 0 };
const int egPieceValue[6] = { 105, 360, 374, 615, 1205, 0 };
const int phaseTable[6]   = {   0,   1,   1,   2,   4,  0 };

const int passedBonusMg[2][8] = { { 0, 10, 10, 30, 50, 80, 120, 0},
                                  { 0, 120, 80, 50, 30, 10, 10, 0} };

const int passedBonusEg[2][8] = { { 0, 11, 11, 37, 62, 102, 155, 0 },
                                  { 0, 155, 102, 62, 37, 11, 11, 0 } };

const int mgPawnPst[64] = {
 //A1                              H1
  0,   0,   0,   0,   0,   0,   0,   0,
-10,  -2,  -4,  -1,   1,   5,  -1,  -7,
 -9,  -4,   4,   8,  10,   1,  -1,  -8,
-10,  -4,   8,  14,  12,   5,  -4, -10,
 -8,  -2,   2,  11,  13,   4,  -4, -10,
 -8,  -2,   4,   8,  10,   4,  -2,  -7,
 -8,  -4,   2,   8,   8,   2,  -4, -10,
  0,   0,   0,   0,   0,   0,   0,   0,
//A8                               H8
};

const int egPawnPst[64] = {
  0,   0,   0,   0,   0,   0,   0,   0,
  2,   0,   0,  -4,  -2,   0,   2,   2,
  2,   0,  -2,  -4,  -2,  -2,   1,   2,
  2,   1,  -2,  -4,  -4,  -2,   0,   2,
  4,   2,   0,  -4,  -4,  -2,   2,   4,
  4,   2,   0,  -4,  -4,   0,   2,   4,
  4,   2,   0,  -2,  -2,   0,   2,   4,
  0,   0,   0,   0,   0,   0,   0,   0,
};

const int mgKnightPst[64] = {
-51, -39, -31, -29, -29, -29, -39, -51,
-39, -21,  -1,   6,   4,  -1, -19, -39,
-29,   4,   9,  17,  17,  11,   6, -29,
-29,  -1,  14,  19,  21,  16,   1, -29,
-29,   6,  14,  21,  19,  16,   4, -29,
-31,  -1,  12,  17,  17,  12,   1, -29,
-41, -21,   1,  -1,   1,   1, -19, -39,
-100, -40, -31, -31, -29, -31, -41, -100,
};

const int egKnightPst[64] = {
-51, -39, -29, -29, -29, -29, -39, -50,
-39, -21,  -1,   4,   4,  -1, -19, -39,
-29,   4,   9,  14,  14,   9,   4, -30,
-29,  -1,  16,  19,  21,  15,  -1, -29,
-29,   6,  15,  21,  19,  16,   5, -29,
-29,  -1,  11,  16,  14,  11,  -1, -29,
-40, -20,   1,  -1,  -1,  -1, -21, -41,
-51, -41, -31, -29, -29, -31, -41, -51,
};

const int mgBishopPst[64] = {
-18, -12,  -8,  -8,  -9, -12, -12, -21,
-12,   7,   2,   0,   3,   2,  10,  -8,
 -8,  12,   8,   8,   8,  10,   7,  -8,
-10,  -2,   8,   8,   9,   8,   0, -12,
-12,   3,   3,   8,   8,   3,   3,  -8,
-12,  -2,   7,   8,  12,   7,   2,  -8,
-12,  -2,  -2,  -2,  -2,   2,   2,  -8,
-22, -12, -11, -12, -12, -12, -10, -22,
};

const int egBishopPst[64] = {
-19, -11,  -9,  -9,  -9, -11, -10, -21,
 -9,   4,  -1,   0,   1,   1,   6,  -9,
 -9,  10,   9,   9,   9,   9,   9,  -9,
 -9,   1,   9,   9,   9,   9,  -1, -11,
 -9,   5,   4,   9,   9,   4,   4,  -9,
 -9,   1,   4,   9,   9,   6,   1,  -9,
-11,  -1,  -1,  -1,   0,   1,   0, -11,
-19, -11, -11, -10, -10, -10, -11, -19,
};

const int mgRookPst[64] = {
 -1,  -1,   1,   6,   6,   1,  -1,  -1,
 -6,  -1,  -1,  -1,  -1,  -1,  -1,  -6,
 -6,  -1,  -1,   0,  -1,  -1,  -1,  -6,
 -4,  -1,  -1,  -1,  -1,  -1,  -1,  -6,
 -5,  -1,   1,   1,  -1,  -1,   0,  -4,
 -4,   0,  -1,   1,   1,   1,   1,  -4,
  5,   9,  11,  11,  10,  11,  11,   6,
  1,   1,   1,   1,   1,   1,   1,   1,
};

const int egRookPst[64] = {
 -1,   0,   1,   6,   6,   1,  -1,  -1,
 -6,  -1,  -1,  -1,   0,  -1,  -1,  -6,
 -6,  -1,  -1,  -1,  -1,  -1,  -1,  -6,
 -4,   0,   0,   0,  -1,  -1,  -1,  -6,
 -4,  -1,   1,   0,  -1,  -1,  -1,  -6,
 -4,   1,   1,   1,   0,   0,   0,  -6,
  6,  11,  11,  11,   9,  11,  10,   4,
  1,   1,   1,   1,   1,   1,   1,   1,
};

const int mgQueenPst[64] = {
-17, -13, -11,  -2,  -8, -13, -13, -23,
-13,  -2,   2,   3,   3,  -3,  -3, -12,
-13,   2,   2,   2,   2,   2,   3, -12,
 -3,  -3,   2,   2,   3,   2,   3,  -5,
 -8,  -3,   2,   2,   3,   8,   1,  -2,
-13,  -3,   8,   6,   8,   8,   3,  -7,
-13,  -3,  -3,  -3,  -3,   3,   3,  -7,
-23, -12,  -7,  -2,  -2,  -7, -12, -17,
};

const int egQueenPst[64] = {
-19, -11, -11,  -4,  -5, -11, -11, -21,
-11,   0,   4,  -1,   1,  -1,  -1, -11,
-11,   4,   5,   4,   4,   5,   0, -10,
 -1,  -1,   4,   4,   5,   6,   1,  -4,
 -6,  -1,   4,   4,   6,   6,   1,  -4,
-11,  -1,   5,   6,   6,   6,   1,  -9,
-11,  -1,  -1,   1,  -1,   1,   1,  -9,
-21,  -9,  -9,  -4,  -4,  -9, -11, -19,
};

const int mgKingPst[64] = {
 19,  29,  10,  -8,  -1,   7,  31,  19,
 19,  19,  -2,  -3,  -3,  -2,  21,  21,
-11, -19, -21, -21, -21, -21, -19, -10,
-20, -30, -31, -41, -41, -31, -29, -21,
-29, -39, -41, -51, -51, -41, -39, -29,
-29, -39, -41, -51, -51, -39, -39, -29,
-29, -39, -39, -49, -49, -39, -39, -29,
-29, -39, -39, -49, -49, -39, -39, -29,
};

const int egKingPst[64] = {
 -50, -30, -30, -30, -30, -30, -30, -50,
 -30, -30,   0,   0,   0,   0, -30, -30,
 -30, -10,  20,  30,  30,  20, -10, -30,
 -30, -10,  30,  40,  40,  30, -10, -30,
 -30, -10,  30,  40,  40,  30, -10, -30,
 -30, -10,  20,  30,  30,  20, -10, -30,
 -30, -20, -10,   0,   0, -10, -20, -30,
 -50, -40, -30, -20, -20, -30, -40, -50
};