// Publius - Didactic public domain bitboard chess engine by Pawel Koziol

#include "types.h"
#include "square.h"
#include "limits.h"
#include "publius.h"
#include "bitboard.h"
#include "bitgen.h"
#include "evaldata.h"
#include "eval.h"
#include "mask.h"

// "Fake" king location, used to initialize
// king attack zone. This has two benefits:
// - increases number of squares taken into
//   account during attack evaluation of
//   a castled king (for Kg1, f3, g3 and h3
//   are included)
// - does not create an additional incentive
//   for playing Kg1-h1

const Square kingRoot[64] = {
    B2, B2, C2, D2, E2, F2, G2, G2,
    B2, B2, C2, D2, E2, F2, G2, G2,
    B3, B3, C3, D3, E3, F3, G3, G3,
    B4, B4, C4, D4, E4, F4, G4, G4,
    B5, B5, C5, D5, E5, F5, G5, G5,
    B6, B6, C6, D6, E6, F6, G6, G6,
    B7, B7, C7, D7, E7, F7, G7, G7,
    B7, B7, C7, D7, E7, F7, G7, G7
};

// eval hashtables
EvalHashTable EvalHash(16384);
sPawnHashEntry PawnTT[PAWN_HASH_SIZE];

Bitboard trappedRookKs[2] = { Paint(G1, H1, H2), Paint(G8, H8, H7) };
Bitboard trappedRookQs[2] = { Paint(B1, A1, A2), Paint(B8, A8, A7) };
Bitboard trappingKingKs[2] = { Paint(F1, G1), Paint(F8, G8) };
Bitboard trappingKingQs[2] = { Paint(C1, B1), Paint(C8, B8) };

int Evaluate(Position* pos, EvalData* e) {

    int score = 0;

    // Try to retrieve the score from the evaluation hashtable
    if (EvalHash.Retrieve(pos->boardHash, &score))
        return score;

    // Init eval data
    e->Clear();
    e->enemyKingZone[White] = GenerateMoves.King(kingRoot[pos->KingSq(Black)]);
    e->enemyKingZone[Black] = GenerateMoves.King(kingRoot[pos->KingSq(White)]);
    e->control[White][Pawn] = GetWPAttacks(pos->Map(White, Pawn));
    e->control[Black][Pawn] = GetBPAttacks(pos->Map(Black, Pawn));
    e->control[White][King] = GenerateMoves.King(pos->KingSq(White));
    e->control[Black][King] = GenerateMoves.King(pos->KingSq(Black));

    // Tempo bonus
    e->Add(pos->GetSideToMove(), tempoMg, tempoEg);

    EvalPawnStructure(pos, e);

    // Evaluate pieces
    for (Color color = White; color < colorNone; ++color) {

        // Bishops pair
        if (pos->Count(color, Bishop) == 2)
            e->Add(color, bishPairMg, bishPairEg);

        // Piece eval
        EvalKnight(pos, e, color);
        EvalBishop(pos, e, color);
        EvalRook(pos, e, color);
        EvalQueen(pos, e, color);

        // Rook trapped by uncastled king (kingside)
        if ((pos->Map(color, King) & trappingKingKs[color]) &&
            (pos->Map(color, Rook) & trappedRookKs[color]))
            e->Add(color, trappedRookMg, trappedRookEg);

        // Rook trapped by uncastled king (queenside)
        if ((pos->Map(color, King) & trappingKingQs[color]) &&
            (pos->Map(color, Rook) & trappedRookQs[color]))
            e->Add(color, trappedRookMg, trappedRookEg);

        // Precalculate board control bitboards
        // for passer and pressure eval
        for (int piece = Pawn; piece <= King; piece++)
            e->allAtt[color] |= e->control[color][piece];
    }

    EvalPasser(pos, e, White);
    EvalPasser(pos, e, Black);

    EvalPressure(pos, e, White);
    EvalPressure(pos, e, Black);

    // Finalize king attacks eval
    EvalKingAttacks(e, White);
    EvalKingAttacks(e, Black);

    // Interpolate between midgame and endgame scores
    score = Interpolate(e);

    // Drawn and drawish endgame evaluation
    int multiplier = 64;
    if (score > 0)
        multiplier = GetDrawMul(pos, White, Black);

    if (score < 0)
        multiplier = GetDrawMul(pos, Black, White);

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

    Bitboard b, frontSpan;
    bool isOpen;

    b = pos->Map(color, Pawn);

    while (b) {
        // Find the next pawn to evaluate
        Square square = PopFirstBit(&b);

        // Pawn material and piece/square table value
        e->AddPawn(color, Params.mgPst[color][Pawn][square],
            Params.egPst[color][Pawn][square]);

        // Asess situation in front of the pawn
        frontSpan = FrontSpan(Paint(square), color);
        isOpen = ((frontSpan & pos->Map(~color, Pawn)) == 0);

        // Doubled pawn
        if (frontSpan & pos->Map(color, Pawn))
            e->AddPawn(color, doubledPawnMg, doubledPawnEg);

        // Strong pawn (phalanx or defended)
        if (Mask.strongPawn[color][square] & pos->Map(color, Pawn))
            e->AddPawn(color, Params.pawnSupport[color][square], 0);

        // Isolated pawn
        else if ((Mask.adjacentFiles[FileOf(square)] & pos->Map(color, Pawn)) == 0)
            e->AddPawn(color, isolPawnMg - 9 * isOpen, isolPawnEg);

        // Backward pawn
        else if ((Mask.support[color][square] & pos->Map(color, Pawn)) == 0)
            e->AddPawn(color, -2 - 6 * isOpen, -1);
    }
}

void EvalKnight(const Position* pos, EvalData* e, Color color) {

    int cnt;
    Bitboard b, mobility, att;

    b = pos->Map(color, Knight);

    while (b) {
        // Find the next knight to evaluate
        Square square = PopFirstBit(&b);

        // Knight material and piece/square table value
        EvalBasic(e, color, Knight, square);

        // Knight mobility
        mobility = GenerateMoves.Knight(square) & ~pos->Occupied();
        cnt = PopCnt(mobility);
        e->Add(color, knightMobMg[cnt], knightMobEg[cnt]);

        // Board control update
        e->control[color][Knight] |= GenerateMoves.Knight(square);

        // Knight attacks on the enemy king zone
        att = GenerateMoves.Knight(square) & e->enemyKingZone[color];
        if (att) {
            e->kingAttUnits[color] += 4 * PopCnt(att & ~e->control[~color][Pawn]);
            e->kingAttUnits[color] += 3 * PopCnt(att & e->control[~color][Pawn]);
        }

    }
}

void EvalBishop(const Position* pos, EvalData* e, Color color) {

    int cnt;
    Bitboard b, mobility, occupancy, att;

    b = pos->Map(color, Bishop);

    while (b) {
        // Find the next bishop to evaluate
        Square square = PopFirstBit(&b);

        // Bishop material and piece/square table value
        EvalBasic(e, color, Bishop, square);

        // Bishop mobility
        mobility = GenerateMoves.Bish(pos->Occupied(), square);
        cnt = PopCnt(mobility);
        e->Add(color, bishMobMg[cnt], bishMobEg[cnt]);

        // Board control update
        e->control[color][Bishop] |= mobility;

        // Bishop attacks on the enemy king zone
        // including attacks through own queen
        occupancy = pos->Occupied() ^ pos->Map(color, Queen);
        att = GenerateMoves.Bish(occupancy, square);
        att &= e->enemyKingZone[color];

        if (att) {
            e->kingAttUnits[color] += 4 * PopCnt(att & ~e->control[~color][Pawn]);
            e->kingAttUnits[color] += 3 * PopCnt(att & e->control[~color][Pawn]);
        }
    }
}

void EvalRook(const Position* pos, EvalData* e, Color color) {

    int cnt;
    Bitboard b, mobility, transparent, occupancy, att, file;

    b = pos->Map(color, Rook);

    while (b) {
        // Find the next rook to evaluate
        Square square = PopFirstBit(&b);

        // Rook material and piece/square table value
        EvalBasic(e, color, Rook, square);

        // Rook mobility
        mobility = GenerateMoves.Rook(pos->Occupied(), square);
        cnt = PopCnt(mobility);
        e->Add(color, rookMobMg[cnt], rookMobEg[cnt]);

        // Board control update
        e->control[color][Rook] |= mobility;

        // Rook's attacks on the enemy king's zone
        // including attacks through own rook or queen
        transparent = pos->MapStraightMovers(color);
        occupancy = pos->Occupied() ^ transparent;
        att = GenerateMoves.Rook(occupancy, square);
        att &= e->enemyKingZone[color];

        if (att) {
            e->kingAttUnits[color] += 6 * PopCnt(att & ~e->control[~color][Pawn]);
            e->kingAttUnits[color] += 4 * PopCnt(att & e->control[~color][Pawn]);
        }

        // Rook on a (semi) open file
        file = FillNorth(Paint(square)) | FillSouth(Paint(square));

        // rook on a closed file
        if (!(file & pos->Map(color, Pawn))) {

            // rook on a semi-open file
            if (file & pos->Map(~color, Pawn))
                e->Add(color, rookHalfMg, rookHalfEg);
            else
                // rook on an open file
                e->Add(color, rookOpenMg, rookOpenEg);
        }

        // Rook on 7th rank attacking pawns or cutting off enemy king
        if (Paint(square) & Mask.rr[color][rank7]) {
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

    Bitboard queenChecks = GenerateMoves.Queen(pos->Occupied(), pos->KingSq(~color));

    b = pos->Map(color, Queen);

    while (b) {
        // Find the next queen to evaluate
        Square square = PopFirstBit(&b);

        // Queen material and piece/square table value
        EvalBasic(e, color, Queen, square);

        // Queen mobility
        mobility = GenerateMoves.Queen(pos->Occupied(), square);
        cnt = PopCnt(mobility);
        e->Add(color, queenMobMg[cnt], queenMobEg[cnt]);

        // Queen check threats
        if (mobility & queenChecks)
            e->kingAttUnits[color] += 2;

        // Board control update
        e->control[color][Queen] |= mobility;

        // Queen attacks on enemy king zone,
        // including attacks through own lesser 
        // pieces moving along the same ray

        // diagonal attacks
        transparent = pos->Map(color, Bishop);
        occupancy = pos->Occupied() ^ transparent;
        att = GenerateMoves.Bish(occupancy, square);

        // straight line attacks
        transparent = pos->Map(color, Rook);
        occupancy = pos->Occupied() ^ transparent;
        att |= GenerateMoves.Rook(occupancy, square);

        // merge attacks with enemy king zone
        att &= e->enemyKingZone[color];

        // queen attack bonuses seem small,
        // but usually they are check threats too,
        // so they are scored twice
        if (att) {
            e->kingAttUnits[color] += 7 * PopCnt(att & ~e->control[~color][Pawn]);
            e->kingAttUnits[color] += 5 * PopCnt(att & e->control[~color][Pawn]);
        }
    }
}

void EvalKing(const Position* pos, EvalData* e, Color color) {

    Bitboard shieldMask, kingsFile, nextFile;

    Square square = pos->KingSq(color);

    // King piece/square table score
    e->AddPawn(color, Params.mgPst[color][King][square],
        Params.egPst[color][King][square]);

    // Penalising open files near the king
    kingsFile = FillNorth(Paint(square)) | FillSouth(Paint(square)) | Paint(square);
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

    shieldMask = GenerateMoves.King(square);
    e->mgPawn[color] += kingPseudoShield * PopCnt(shieldMask & pos->Map(color, Pawn));

    shieldMask = ForwardOf(shieldMask, color);
    e->mgPawn[color] += kingPseudoShield * PopCnt(shieldMask & pos->Map(color, Pawn));
}

void EvalPasser(const Position* pos, EvalData* e, Color color) {

    Bitboard b, stop;
    Color oppo = ~color;

    b = pos->Map(color, Pawn);

    while (b) {
        // Find the next pawn to evaluate
        Square square = PopFirstBit(&b);
        stop = ForwardOf(Paint(square), color);

        // Passed pawn
        if (!(Mask.passed[color][square] & pos->Map(~color, Pawn))) {
            int mul = 100;
            if (stop & e->allAtt[color]) mul += 33;
            if (stop & e->allAtt[oppo]) mul -= 33;
            if (stop & pos->Occupied()) mul -= 15;
            e->mg[color] += passedBonusMg[color][RankOf(square)] * mul / 100;
            e->eg[color] += passedBonusEg[color][RankOf(square)] * mul / 100;
        }
    }
}

void EvalPressure(Position* pos, EvalData* e, Color side) {

    Color oppo;
    Square sq;
    int pieceType, pressureMg, pressureEg;
    Bitboard enemyPieces, ctrl, hang;

    pressureMg = 0;
    pressureEg = 0;
    oppo = ~side;
    enemyPieces = pos->Map(oppo);

    // bishop on knight attacks
    if (pos->Map(oppo, Bishop) & e->control[side][Knight]) {
        pressureMg += minorOnMinor;
        pressureEg += minorOnMinor;
    }

    // knight on bishop attacks
    if (pos->Map(oppo, Knight) & e->control[side][Bishop]) {
        pressureMg += minorOnMinor;
        pressureEg += minorOnMinor;
    }

    // detect hanging pieces
    ctrl = e->allAtt[side] & ~e->allAtt[oppo];
    hang = (enemyPieces & ctrl) | (enemyPieces & e->control[side][Pawn]);

    // evaluate enemy pieces, hanging and attacked
    while (hang) {
        sq = PopFirstBit(&hang);
        pieceType = pos->PieceTypeOnSq(sq);
        pressureMg += mgPressure[pieceType];
        pressureEg += egPressure[pieceType];
    }

    e->Add(side, pressureMg, pressureEg);
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

// King attack calculation, relying on the number
// of "attack units" and table lookup. Attack units
// are awarded for eah possibility of movig a piece
// to squares adjacent to the opponent's king.
void EvalKingAttacks(EvalData* e, Color color) {

    int att = Mask.kingAttack[std::min(e->kingAttUnits[color], 255)];
    e->Add(color, std::min(att, 600), 0);
}

int Interpolate(EvalData* e) {

    // Sum all the eval factors
    int mgScore = e->mg[White] - e->mg[Black];
    int egScore = e->eg[White] - e->eg[Black];

    // Score interpolation
    int mgPhase = std::min(24, e->phase);
    int egPhase = 24 - mgPhase;
    return ((mgScore * mgPhase + egScore * egPhase) / MaxGamePhase);
}