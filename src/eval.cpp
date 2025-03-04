#include "color.h"
#include "square.h"
#include "limits.h"
#include "publius.h"
#include "bitboard.h"
#include "bitgen.h"
#include "evaldata.h"
#include "eval.h"
#include "mask.h"
#include "piece.h"

int Evaluate(Position *pos, EvalData *e) {

    // Init eval data
    e->Clear();
    e->enemyKingZone[White] = GenerateMoves.King(pos->KingSq(Black));
    e->enemyKingZone[Black] = GenerateMoves.King(pos->KingSq(White));

    // Evaluate pieces and pawns
    for (Color color = White; color < colorNone; ++color) {

        // Bishops pair
        if (pos->Count(color, Bishop) == 2) {
            e->Add(color, 40, 60);
        }

        // Piece eval
        EvalPawn(pos, e, color);
        EvalKnight(pos, e, color);
        EvalBishop(pos, e, color);
        EvalRook(pos, e, color);
        EvalQueen(pos, e, color);
        EvalKing(pos, e, color);
    }

    // Finalize king attacks eval
    EvalKingAttacks(e, White);
    EvalKingAttacks(e, Black);

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
  return pos->GetSideToMove() == White ? score : -score;
}

void EvalPawn(Position* pos, EvalData* e, Color color) {

    Bitboard b, span, support;

    b = pos->Map(color, Pawn);

    while (b) {
        Square sq = PopFirstBit(&b);

        // Pawn material and piece/square table value
        EvalBasic(e, color, Pawn, sq);

        // Doubled pawn
        span = FrontSpan(Paint(sq), color);
        if (span & pos->Map(color, Pawn)) {
            e->Add(color, -9, -9);
        }

        // Isolated pawn
        if ((Mask.adjacent[FileOf(sq)] & pos->Map(color, Pawn)) == 0) {
            e->Add(color, -10, -18);
        }

        // Passed pawn
        if (!(Mask.passed[color][sq] & pos->Map(~color, Pawn))) {
            e->mg[color] += passedBonusMg[color][RankOf(sq)];
            e->eg[color] += passedBonusEg[color][RankOf(sq)];
        }
    }
}

void EvalKnight(Position* pos, EvalData* e, Color color) {

    int cnt;
    Bitboard b, mobility;

    b = pos->Map(color,Knight);

    while (b) {
        Square sq = PopFirstBit(&b);

        // Knight material and piece/square table value
        EvalBasic(e, color, Knight, sq);

        // Knight mobility
        mobility = GenerateMoves.Knight(sq) & ~pos->Occupied();
        cnt = PopCnt(mobility);
        e->Add(color, knightMobMg[cnt], knightMobEg[cnt] );

        // Knight attacks on the enemy king zone
        if (GenerateMoves.Knight(sq) & e->enemyKingZone[color])
            e->minorAttacks[color]++;
    }
}

void EvalBishop(Position* pos, EvalData* e, Color color) {

    int cnt;
    Bitboard b, mobility, occupancy, att;

    b = pos->Map(color, Bishop);

    while (b) {
        Square sq = PopFirstBit(&b);

        // Bishop material and piece/square table value
        EvalBasic(e, color, Bishop, sq);

        // Bishop mobility
        mobility = GenerateMoves.Bish(pos->Occupied(), sq);
        cnt = PopCnt(mobility);
        e->Add(color, bishMobMg[cnt], bishMobEg[cnt]);

        // Bishop attacks on the enemy king zone
        // including attacks through own queen
        occupancy = pos->Occupied() ^ pos->Map(color, Queen);
        att = GenerateMoves.Bish(occupancy, sq);
        if (att & e->enemyKingZone[color])
            e->minorAttacks[color]++;
    }
}

void EvalRook(Position* pos, EvalData* e, Color color) {

    int cnt;
    Bitboard b, mobility, transparent, occupancy, att, file;

    b = pos->Map(color, Rook);

    while (b) {
        Square sq = PopFirstBit(&b);

        // Rook material and piece/square table value
        EvalBasic(e, color, Rook, sq);

        // Rook mobility
        mobility = GenerateMoves.Rook(pos->Occupied(), sq);
        cnt = PopCnt(mobility);
        e->Add(color, rookMobMg[cnt], rookMobEg[cnt]);

        // Rook's attacks on the enemy king's zone
        // including attacks through own rook or queen
        transparent = pos->MapStraightMovers(color);
        occupancy = pos->Occupied() ^ transparent;
        att = GenerateMoves.Rook(occupancy, sq);
        if (att & e->enemyKingZone[color])
            e->rookAttacks[color]++;

        // Rook's file (closed, semi-open, open)
        file = FillNorth(b) | FillSouth(b);

        if (file & pos->Map(color, Pawn)) {
            e->Add(color, -6, -6);     // rook on a closed file
        }
        else
        {
            if (file & pos->Map(~color, Pawn))
                e->Add(color, 6, 6);   // rook on a semi-open file
            else
                e->Add(color, 12, 12); // rook on an open file
        }

        // Rook on 7th rank attacking pawns or cutting off enemy king
        if (Paint(sq) & Mask.rr[color][rank7]) {
            if (pos->Map(~color, Pawn) & Mask.rr[color][rank7]
                || pos->Map(~color, King) & Mask.rr[color][rank8]) {
                e->Add(color, 12, 30);
            }
        }
    }
}

void EvalQueen(Position* pos, EvalData* e, Color color) {

    int cnt;
    Bitboard b, mobility, transparent, occupancy, att;

    b = pos->Map(color, Queen);

    while (b) {
        Square sq = PopFirstBit(&b);

        // Queen material and piece/square table value
        EvalBasic(e, color, Queen, sq);

        // Queen mobility
        mobility = GenerateMoves.Queen(pos->Occupied(), sq);
        cnt = PopCnt(mobility);
        e->Add(color, queenMobMg[cnt], queenMobEg[cnt]);

        // Queen attacks on enemy king zone
        // including attacks through own lesser pieces
        // moving along the same ray

        // diagonal attacks
        transparent = pos->Map(color, Bishop);
        occupancy = pos->Occupied() ^ transparent;
        att = GenerateMoves.Bish(occupancy, sq);

        // straight line attacks
        transparent = pos->Map(color, Rook);
        occupancy = pos->Occupied() ^ transparent;
        att |= GenerateMoves.Rook(occupancy, sq);
        
        // register the attack
        if (att & e->enemyKingZone[color])
            e->queenAttacks[color]++;
    }
}

void EvalKing(Position* pos, EvalData* e, Color color) {

    Bitboard b, mobility;

    b = pos->Map(color, King);

    while (b) {
        Square sq = PopFirstBit(&b);
        
        // King piece/square table score
        EvalBasic(e, color, King, sq);
        
        // (sorry equivalent of) king's pawn shield
        // (pawns closer to the king are counted twice)

        mobility = GenerateMoves.King(sq);
        e->mg[color] += 8 * PopCnt(mobility & pos->Map(color, Pawn));

        mobility = ForwardOf(mobility, color);
        e->mg[color] += 8 * PopCnt(mobility & pos->Map(color, Pawn));
    }
}

void EvalBasic(EvalData *e, const Color color, const int piece, const int sq) {

    e->phase += phaseTable[piece];
    e->Add(color, mgPieceValue[piece], egPieceValue[piece]);
    e->Add(color, Params.mgPst[color][piece][sq], 
                  Params.egPst[color][piece][sq]);
}

void EvalKingAttacks(EvalData* e, Color color) {

    int result = 2 * e->queenAttacks[color] * e->rookAttacks[color] * e->minorAttacks[color];
    result += 17 * e->queenAttacks[color] * e->rookAttacks[color];
    result += 12 * e->queenAttacks[color] * e->minorAttacks[color];
    result +=  9 * e->rookAttacks[color] * e->minorAttacks[color];
    result += 3 * e->minorAttacks[color] * e->minorAttacks[color];
    result += 1 * e->queenAttacks[color];
    result += 3 * e->rookAttacks[color];
    result -= 3 * e->minorAttacks[color];

    e->Add(color, 400 * result / 100, 0);
}