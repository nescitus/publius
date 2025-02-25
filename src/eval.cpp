#include <assert.h>
#include <stdio.h>
#include "publius.h"
#include "bitboard.h"
#include "bitgen.h"
#include "eval.h"
#include "mask.h"
#include "piece.h"
#include "square.h"

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

void EvalSinglePiece(Position *pos, evalData *e, Color color, int piece) {

    Bitboard b, file;

    b = pos->Map(color, piece);
    
    while (b) {
        Square sq = PopFirstBit(&b);
        e->phase += phaseTable[piece];
        e->mg[color] += mgPieceValue[piece];
        e->eg[color] += egPieceValue[piece];
        e->mg[color] += mgTable[color][piece][sq];
        e->eg[color] += egTable[color][piece][sq];

        Bitboard mobility;

        if (piece == Pawn) {

            // passed pawn

            if (!(Mask.passed[color][sq] & pos->Map(~color, Pawn))) {
                e->mg[color] += passedBonus[color][RankOf(sq)];
                e->eg[color] += (passedBonus[color][RankOf(sq)] * 5) / 4;
            }
        }

        if (piece == Knight) {
            mobility = GenerateMoves.Knight(sq) &~pos->Occupied();
            e->mg[color] += 4 * ( PopCnt(mobility) - 4);
            e->eg[color] += 4 * ( PopCnt(mobility) - 4);
        }

        if (piece == Bishop) {
            mobility = GenerateMoves.Bish(pos->Occupied(), sq);
            e->mg[color] += 5 * (PopCnt(mobility) - 6);
            e->eg[color] += 5 * (PopCnt(mobility) - 6);
        }

        if (piece == Rook) {
            mobility = GenerateMoves.Rook(pos->Occupied(), sq);
            e->mg[color] += 2 * (PopCnt(mobility) - 7);
            e->eg[color] += 4 * (PopCnt(mobility) - 7);

            file = FillNorth(b) | FillSouth(b);

            // rook on a closed file
            if (file & pos->Map(color, Pawn)) {
                e->mg[color] -= 5;
                e->eg[color] -= 5;
            }
            else
            {
                // rook on a semi-open file
                if (file & pos->Map(~color, Pawn)) {
                    e->mg[color] += 5;
                    e->eg[color] += 5;
                }
                // rook on an open file
                else
                {
                    e->mg[color] += 10;
                    e->eg[color] += 10;
                }
            }
        }

        if (piece == Queen) {
            mobility = GenerateMoves.Bish(pos->Occupied(), sq) | GenerateMoves.Rook(pos->Occupied(), sq);
            e->mg[color] += 1 * (PopCnt(mobility) - 13);
            e->eg[color] += 2 * (PopCnt(mobility) - 13);
        }

        if (piece == King) {
            mobility = GenerateMoves.King(sq);
            e->mg[color] += 8 * PopCnt(mobility & pos->Map(color, Pawn));

            mobility = ForwardOf(mobility, color);
            e->mg[color] += 8 * PopCnt(mobility & pos->Map(color, Pawn));
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