#include <assert.h>
#include <stdio.h>
#include "publius.h"
#include "eval.h"

// TODO: Params class 
int mgTable[2][6][64];
int egTable[2][6][64];

int Evaluate(Position *pos, evalData *e) {

    // Clear eval

    e->phase = 0;

    for (Color color = White; color < colorNone; ++color) {
        e->mg[color] = 0;
        e->eg[color] = 0;
    }

    // Evaluate pieces and pawns

    for (Color color = White; color < colorNone; ++color) {

        if (pos->GetCount(color, Bishop) == 2) {
            e->mg[color] += 40;
            e->eg[color] += 60;
        }

        for (int type = Pawn; type < noPieceType; type++)
	        EvalSinglePiece(pos, e, color, type);
    }

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

void EvalSinglePiece(Position *pos, evalData *e, Color side, int piece) {

    Bitboard b, file;

    b = pos->Map(side, piece);
    
    while (b) {
        Square sq = PopFirstBit(&b);
        e->phase += phaseTable[piece];
        e->mg[side] += mgPieceValue[piece];
        e->eg[side] += egPieceValue[piece];
        e->mg[side] += mgTable[side][piece][sq];
        e->eg[side] += egTable[side][piece][sq];

        Bitboard mobility;

        if (piece == Pawn) {

            // passed pawn

            if (!(Mask.passed[side][sq] & pos->Map(~side, Pawn))) {
                e->mg[side] += passedBonus[side][RankOf(sq)];
                e->eg[side] += (passedBonus[side][RankOf(sq)] * 5) / 4;
            }
        }

        if (piece == Knight) {
            mobility = GenerateMoves.Knight(sq) &~pos->Occupied();
            e->mg[side] += 4 * ( PopCnt(mobility) - 4);
            e->eg[side] += 4 * ( PopCnt(mobility) - 4);
        }

        if (piece == Bishop) {
            mobility = GenerateMoves.Bish(pos->Occupied(), sq);
            e->mg[side] += 5 * (PopCnt(mobility) - 6);
            e->eg[side] += 5 * (PopCnt(mobility) - 6);
        }

        if (piece == Rook) {
            mobility = GenerateMoves.Rook(pos->Occupied(), sq);
            e->mg[side] += 2 * (PopCnt(mobility) - 7);
            e->eg[side] += 4 * (PopCnt(mobility) - 7);

            file = FillNorth(b) | FillSouth(b);

            // rook on a closed file
            if (file & pos->Map(side, Pawn)) {
                e->mg[side] -= 5;
                e->eg[side] -= 5;
            }
            else
            {
                // rook on a semi-open file
                if (file & pos->Map(~side, Pawn)) {
                    e->mg[side] += 5;
                    e->eg[side] += 5;
                }
                // rook on an open file
                else
                {
                    e->mg[side] += 10;
                    e->eg[side] += 10;
                }
            }
        }

        if (piece == Queen) {
            mobility = GenerateMoves.Bish(pos->Occupied(), sq) | GenerateMoves.Rook(pos->Occupied(), sq);
            e->mg[side] += 1 * (PopCnt(mobility) - 13);
            e->eg[side] += 2 * (PopCnt(mobility) - 13);
        }

        if (piece == King) {
            mobility = GenerateMoves.King(sq);
            e->mg[side] += 8 * PopCnt(mobility & pos->Map(side, Pawn));

            mobility = ForwardOf(mobility, side);
            e->mg[side] += 8 * PopCnt(mobility & pos->Map(side, Pawn));
        }
    }
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