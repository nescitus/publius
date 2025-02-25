#include <assert.h>
#include <stdio.h>
#include "color.h"
#include "square.h"
#include "publius.h"
#include "bitboard.h"
#include "bitgen.h"
#include "eval.h"
#include "mask.h"
#include "piece.h"

// TODO: Params class 
int mgTable[2][6][64];
int egTable[2][6][64];

int Evaluate(Position *pos, evalData *e) {

    // Clear eval data

    e->Clear();
    e->enemyKingZone[White] = GenerateMoves.King(pos->KingSq(Black));
    e->enemyKingZone[Black] = GenerateMoves.King(pos->KingSq(White));

    // Evaluate pieces and pawns

    for (Color color = White; color < colorNone; ++color) {

        if (pos->GetCount(color, Bishop) == 2) {
            e->Add(color, 40, 60);
        }

        EvalPawn(pos, e, color);
        EvalKnight(pos, e, color);
        EvalBishop(pos, e, color);
        EvalRook(pos, e, color);
        EvalQueen(pos, e, color);
        EvalKing(pos, e, color);
    }

    EvalAttacks(e, White);
    EvalAttacks(e, Black);

    // Sum all the eval factors

    int mgScore = e->mg[White] - e->mg[Black];
    int egScore = e->eg[White] - e->eg[Black];

    // Score interpolation

    int mgPhase = std::min(24, e->phase);
    int egPhase = 24 - mgPhase;
    int score = (((mgScore * mgPhase) + (egScore * egPhase)) / 24);

  // Drawn and drawish endgame evaluation

  int multiplier = 64;
  if (score > 0) {
      multiplier = GetDrawMul(pos, White, Black);
  }

  if (score < 0) {
      multiplier = GetDrawMul(pos, Black, White);
  }

  score = (score * multiplier) / 64;

  // Make sure eval doesn't exceed mate score

  score = Clip(score, EvalLimit);

  // Return score relative to the side to move

  return pos->GetSide() == White ? score : -score;
}

void EvalPawn(Position* pos, evalData* e, Color color) {

    Bitboard b, file;

    b = pos->Map(color, Pawn);

    while (b) {
        Square sq = PopFirstBit(&b);
        EvalBasic(e, color, Pawn, sq);

        // passed pawn

        if (!(Mask.passed[color][sq] & pos->Map(~color, Pawn))) {
            e->mg[color] += passedBonusMg[color][RankOf(sq)];
            e->eg[color] += passedBonusEg[color][RankOf(sq)];
        }
    }
}

void EvalKnight(Position* pos, evalData* e, Color color) {

    Bitboard b, mobility;

    b = pos->Map(color,Knight);

    while (b) {
        Square sq = PopFirstBit(&b);
        EvalBasic(e, color, Knight, sq);

        mobility = GenerateMoves.Knight(sq) & ~pos->Occupied();
        e->mg[color] += 4 * (PopCnt(mobility) - 4);
        e->eg[color] += 4 * (PopCnt(mobility) - 4);

        if (GenerateMoves.Knight(sq) & e->enemyKingZone[color])
            e->minorAttacks[color]++;
    }
}

void EvalBishop(Position* pos, evalData* e, Color color) {

    Bitboard b, att, mobility;

    b = pos->Map(color, Bishop);

    while (b) {
        Square sq = PopFirstBit(&b);
        EvalBasic(e, color, Bishop, sq);

        mobility = GenerateMoves.Bish(pos->Occupied(), sq);
        e->mg[color] += 5 * (PopCnt(mobility) - 6);
        e->eg[color] += 5 * (PopCnt(mobility) - 6);

        att = GenerateMoves.Bish(pos->Occupied() ^ pos->Map(color, Queen), sq);
        if (att & e->enemyKingZone[color])
            e->minorAttacks[color]++;
    }

}

void EvalRook(Position* pos, evalData* e, Color color) {

    Bitboard b, att, file, mobility;

    b = pos->Map(color, Rook);

    while (b) {
        Square sq = PopFirstBit(&b);
        EvalBasic(e, color, Rook, sq);

        mobility = GenerateMoves.Rook(pos->Occupied(), sq);
        e->mg[color] += 2 * (PopCnt(mobility) - 7);
        e->eg[color] += 4 * (PopCnt(mobility) - 7);

        att = GenerateMoves.Rook(pos->Occupied() ^ (pos->Map(color, Queen) | pos->Map(color, Rook)), sq);
        if (att & e->enemyKingZone[color])
            e->rookAttacks[color]++;

        file = FillNorth(b) | FillSouth(b);

        if (file & pos->Map(color, Pawn)) {
            e->Add(color, -5, -5);     // rook on a closed file
        }
        else
        {
            if (file & pos->Map(~color, Pawn))
                e->Add(color, 5, 5);   // rook on a semi-open file
            else
                e->Add(color, 10, 10); // rook on an open file
        }
    }
}

void EvalQueen(Position* pos, evalData* e, Color color) {

    Bitboard b, att, mobility;

    b = pos->Map(color, Queen);

    while (b) {
        Square sq = PopFirstBit(&b);
        EvalBasic(e, color, Queen, sq);

        mobility = GenerateMoves.Queen(pos->Occupied(), sq);
        e->mg[color] += 1 * (PopCnt(mobility) - 13);
        e->eg[color] += 2 * (PopCnt(mobility) - 13);

        att = GenerateMoves.Bish(pos->Occupied() ^ pos->Map(color, Bishop), sq);

        att |= GenerateMoves.Rook(pos->Occupied() ^ pos->Map(color, Rook), sq);
        if (att & e->enemyKingZone[color])
            e->queenAttacks[color]++;
    }
}

void EvalKing(Position* pos, evalData* e, Color color) {

    Bitboard b, mobility;

    b = pos->Map(color, King);

    while (b) {
        Square sq = PopFirstBit(&b);
        EvalBasic(e, color, King, sq);

        // king's pawn shield
        // (pawns closer to the king are counted twice)

        mobility = GenerateMoves.King(sq);
        e->mg[color] += 8 * PopCnt(mobility & pos->Map(color, Pawn));

        mobility = ForwardOf(mobility, color);
        e->mg[color] += 8 * PopCnt(mobility & pos->Map(color, Pawn));

    }
}

void EvalBasic(evalData *e, Color color, int piece, int sq) {

    e->phase += phaseTable[piece];
    e->Add(color, mgPieceValue[piece], egPieceValue[piece]);
    e->Add(color, mgTable[color][piece][sq], egTable[color][piece][sq]);

}

void EvalAttacks(evalData* e, Color color) {

    int result = 16 * e->queenAttacks[color] * e->rookAttacks[color];
    result += 12 * e->queenAttacks[color] * e->minorAttacks[color];
    result +=  8 * e->rookAttacks[color] * e->minorAttacks[color];
    result += 4 * e->minorAttacks[color] * e->minorAttacks[color];

    e->Add(color, 4 * result, 0);
}

int GetDrawMul(Position *pos, Color strong, Color weak) {

    // stronger side has no pawns
    if (pos->GetCount(strong, Pawn) == 0) {

        // With no pawns, a single minor piece 
        // cannot win (KK, KBK, KNK, KBKP, KNKP)

        if (pos->GetMajorCount(strong) == 0
        && pos->GetMinorCount(strong) <= 1)
            return 0;
    }

    return 64;
}