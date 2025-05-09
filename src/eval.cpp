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
#include <iostream>
#include <algorithm>

EvalHashTable EvalHash(1024);
sPawnHashEntry PawnTT[PAWN_HASH_SIZE];

int Evaluate(Position* pos, EvalData* e) {

    int score = 0;

    // Try to retrieve the score from the evaluation hashtable
    if (EvalHash.Retrieve(pos->boardHash, &score)) {
        return score;
    }

    // Init eval data
    e->Clear();
    e->enemyKingZone[White] = GenerateMoves.King(pos->KingSq(Black));
    e->enemyKingZone[Black] = GenerateMoves.King(pos->KingSq(White));

    // Tempo bonus
    e->Add(pos->GetSideToMove(), tempoMg, tempoEg);

    EvalPawnStructure(pos, e);

    // Evaluate pieces
    for (Color color = White; color < colorNone; ++color) {

        // Bishops pair
        if (pos->Count(color, Bishop) == 2) {
            e->Add(color, bishPairMg, bishPairEg);
        }

        // Piece eval
        EvalKnight(pos, e, color);
        EvalBishop(pos, e, color);
        EvalRook(pos, e, color);
        EvalQueen(pos, e, color);
    }

    // Finalize king attacks eval
    EvalKingAttacks(e, White);
    EvalKingAttacks(e, Black);

    // Interpolate between midgame and endgame scores
    score = Interpolate(e);

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
  score = std::clamp(score, -EvalLimit, EvalLimit);

  // Make score relative to the side to move
  if (pos->GetSideToMove() == Black)
      score = -score;

  // Save the score in the evaluation hashtable
  EvalHash.Save(pos->boardHash, score);

  return score;
}

void EvalPawnStructure(const Position* pos, EvalData* e) {

    int addr = pos->pawnHash % PAWN_HASH_SIZE;

    if (PawnTT[addr].key == pos->pawnHash) {
        e->mgPawn[White] = PawnTT[addr].mg[White];
        e->mgPawn[Black] = PawnTT[addr].mg[Black];
        e->egPawn[White] = PawnTT[addr].eg[White];
        e->egPawn[Black] = PawnTT[addr].eg[Black];
    }
    else
    {
        EvalPawn(pos, e, White);
        EvalPawn(pos, e, Black);
        EvalKing(pos, e, White);
        EvalKing(pos, e, Black);

        PawnTT[addr].key = pos->pawnHash;
        PawnTT[addr].mg[White] = e->mgPawn[White];
        PawnTT[addr].mg[Black] = e->mgPawn[Black];
        PawnTT[addr].eg[White] = e->egPawn[White];
        PawnTT[addr].eg[Black] = e->egPawn[Black];
    }

    e->mg[White] += e->mgPawn[White];
    e->eg[White] += e->egPawn[White];
    e->mg[Black] += e->mgPawn[Black];
    e->eg[Black] += e->egPawn[Black];
}

void EvalPawn(const Position* pos, EvalData* e, Color color) {

    Bitboard b, span;

    b = pos->Map(color, Pawn);

    while (b) {
        // Find the next pawn to evaluate
        Square sq = PopFirstBit(&b);

        // Pawn material and piece/square table value
        e->AddPawn(color, Params.mgPst[color][Pawn][sq],
                          Params.egPst[color][Pawn][sq]);

        // Doubled pawn
        span = FrontSpan(Paint(sq), color);
        if (span & pos->Map(color, Pawn)) {
            e->AddPawn(color, doubledPawnMg, doubledPawnEg);
        }

        // Strong pawn (phalanx or defended)
        if (Mask.strongPawn[color][sq] & pos->Map(color, Pawn)) {
            e->AddPawn(color, Params.pawnSupport[color][sq], 0);
        }

        // Isolated pawn
        else if ((Mask.adjacentFiles[FileOf(sq)] & pos->Map(color, Pawn)) == 0) {
            e->AddPawn(color, isolPawnMg, isolPawnEg);
        }

        // Passed pawn
        if (!(Mask.passed[color][sq] & pos->Map(~color, Pawn))) {
            e->mgPawn[color] += passedBonusMg[color][RankOf(sq)];
            e->egPawn[color] += passedBonusEg[color][RankOf(sq)];
        }
    }
}

void EvalKnight(const Position* pos, EvalData* e, Color color) {

    int cnt;
    Bitboard b, mobility;

    b = pos->Map(color,Knight);

    while (b) {
        // Find the next knight to evaluate
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

void EvalBishop(const Position* pos, EvalData* e, Color color) {

    int cnt;
    Bitboard b, mobility, occupancy, att;

    b = pos->Map(color, Bishop);

    while (b) {
        // Find the next bishop to evaluate
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

void EvalRook(const Position* pos, EvalData* e, Color color) {

    int cnt;
    Bitboard b, mobility, transparent, occupancy, att, file;

    b = pos->Map(color, Rook);

    while (b) {
        // Find the next rook to evaluate
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
        file = FillNorth(Paint(sq)) | FillSouth(Paint(sq));

        // rook on a closed file
        if (file & pos->Map(color, Pawn)) {
            e->Add(color, rookClosedMg, rookClosedEg);
        }
        else
        {
            // rook on a semi-open file
            if (file & pos->Map(~color, Pawn))
                e->Add(color, rookHalfMg, rookHalfEg);   
            else
                // rook on an open file
                e->Add(color, rookOpenMg, rookOpenEg); 
        }

        // Rook on 7th rank attacking pawns or cutting off enemy king
        if (Paint(sq) & Mask.rr[color][rank7]) {
            if (pos->Map(~color, Pawn) & Mask.rr[color][rank7] ||
                pos->Map(~color, King) & Mask.rr[color][rank8]) {
                e->Add(color, rook7thMg, rook7thEg);
            }
        }
    }
}

void EvalQueen(const Position* pos, EvalData* e, Color color) {

    int cnt;
    Bitboard b, mobility, transparent, occupancy, att;

    b = pos->Map(color, Queen);

    while (b) {
        // Find the next queen to evaluate
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

void EvalKing(const Position* pos, EvalData* e, Color color) {

    Bitboard shieldMask, kingsFile, nextFile;

    Square sq = pos->KingSq(color);
        
    // King piece/square table score
    e->AddPawn(color, Params.mgPst[color][King][sq],
                      Params.egPst[color][King][sq]);

    // Penalising open files near the king
    kingsFile = FillNorth(Paint(sq)) | FillSouth(Paint(sq)) | Paint(sq);
    if ((kingsFile & pos->Map(color, Pawn)) == 0)
        e->mgPawn[color] += kingOpenFilePenalty;

    nextFile = EastOf(kingsFile);
    if (nextFile) {
        if ((nextFile & pos->Map(color, Pawn)) == 0)
            e->mgPawn[color] += kingNearOpenPenalty;
    }

    nextFile = WestOf(kingsFile);
    if (nextFile) {
        if ((nextFile & pos->Map(color, Pawn)) == 0)
            e->mgPawn[color] += kingNearOpenPenalty;
    }
        
    // (sorry equivalent of) king's pawn shield
    // (pawns closer to the king are counted twice)

    shieldMask = GenerateMoves.King(sq);
    e->mgPawn[color] += kingPseudoShield * PopCnt(shieldMask & pos->Map(color, Pawn));

    shieldMask = ForwardOf(shieldMask, color);
    e->mgPawn[color] += kingPseudoShield * PopCnt(shieldMask & pos->Map(color, Pawn));
}

// Operations repeated while evaluating any piece:
// adding material value, piece/square tables score
// and calculating the game phase.
void EvalBasic(EvalData* e, const Color color, const int piece, const int sq) {

    e->phase += phaseTable[piece];
    //e->Add(color, mgPieceValue[piece], egPieceValue[piece]); // merged with pst
    e->Add(color, Params.mgPst[color][piece][sq], 
                  Params.egPst[color][piece][sq]);
}

// Simple king attack calculation, relying just on the number
// of attackers and their strength. More attacking pieces = good,
// more heavy attackers = better, attack with just minor pieces = meh.
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

int Interpolate(EvalData* e) {

    // Sum all the eval factors
    int mgScore = e->mg[White] - e->mg[Black];
    int egScore = e->eg[White] - e->eg[Black];

    // Score interpolation
    int mgPhase = std::min(24, e->phase);
    int egPhase = 24 - mgPhase;
    return (((mgScore * mgPhase) + (egScore * egPhase)) / MaxGamePhase);
}