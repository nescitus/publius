// Publius - Didactic public domain bitboard chess engine by Pawel Koziol

#include <iomanip> // for std::setw, std::setprecision
#include "types.h"
#include "square.h"
#include "publius.h"
#include "evaldata.h"
#include "eval.h"

void Parameters::Init(void) {

    for (Color color = White; color < colorNone; ++color) {
        for (Square square = A1; square < sqNone; ++square) {

            pawnSupport[color][RelativeSq(color, square)] = S(p_support[square],0);

            pst[color][Pawn][RelativeSq(color, square)] = pieceValue[Pawn] + pawnPst[square];
            pst[color][Knight][RelativeSq(color, square)] = pieceValue[Knight] + knightPst[square];
            pst[color][Bishop][RelativeSq(color, square)] = pieceValue[Bishop] + bishopPst[square];
            pst[color][Rook][RelativeSq(color, square)] = pieceValue[Rook] + rookPst[square];
            pst[color][Queen][RelativeSq(color, square)] = pieceValue[Queen] + queenPst[square];
            pst[color][King][RelativeSq(color, square)] = kingPst[square];
        }
    }

#ifdef USE_TUNING
    /**
    PrintPst(King);
    Position pos;
    Tuner.Init(4000);
    double currentFit = Tuner.TexelFit(&pos);
    
    for (Square s = A1; s <= H8; ++s) 
        currentFit = TuneSingleSquare(&pos, King, s,  currentFit);
    
    PrintPst(King);
    /**/
#endif
}

#ifdef USE_TUNING   
double Parameters::TuneSingleSquare(Position* pos, int piece, Square s, double currentFit) {

    currentFit = TryChangeMgPst(pos, piece, s, 1, currentFit);
    currentFit = TryChangeMgPst(pos, piece, s, -1, currentFit);
    currentFit = TryChangeMgPst(pos, piece, s, 1, currentFit);
    currentFit = TryChangeMgPst(pos, piece, s, -1, currentFit);

    // Due to symmetry, endgame tables are updated twice;
    // we compensate for that by calling TryChangeMgPst()
    // more often.
    currentFit = TryChangeEgPst(pos, piece, s, 1, currentFit);
    currentFit = TryChangeEgPst(pos, piece, s, -1, currentFit);
    return currentFit;
}

void Parameters::PrintPst(int piece) {
    std::cout << "{\n  ";
    for (Square sq = A1; sq <= H8; ++sq) {

        // revert initialization by subtracting material value
        int v = pst[White][piece][sq] - pieceValue[piece];
        int mg = ScoreMG(v);
        int eg = ScoreEG(v);

        std::cout << "S(" << std::setw(3) << mg << "," << std::setw(3) << eg << ")";

        // print commas except after the last element
        if (sq != H8) std::cout << ", ";

        // newline after every 8 squares
        if (FileOf(sq) == 7 && sq != H8)
            std::cout << "\n  ";
    }
    std::cout << "\n};\n";
}

double Parameters::TryChangeMgPst(Position *pos, int piece, Square sq, int delta, double baselineLoss) { 
    
    int oldWeight = Params.pst[White][piece][sq];
    int mgWeight = ScoreMG(oldWeight);
    int egWeight = ScoreEG(oldWeight);
    int mirror = MirrorRank(sq);

    Params.pst[White][piece][sq] = MakeScore(mgWeight + delta, egWeight);
    Params.pst[Black][piece][mirror] = MakeScore(mgWeight + delta, egWeight);

    double newLoss = Tuner.TexelFit(pos);

    if (newLoss + 1e-12 < baselineLoss) {
        std::cout << "MG success: piece " << piece << " sq " << SquareName(sq)
                  << " delta " << delta << " -> loss " << newLoss
                  << " (was " << baselineLoss << ")\n";
        return newLoss; // keep new values
    }
    else {
        pst[White][piece][sq] = oldWeight;
        pst[Black][piece][mirror] = oldWeight;
        std::cout << "MG no-improve: loss " << baselineLoss << "\n";
        return baselineLoss; // revert
    }
}

double Parameters::TryChangeEgPst(Position* pos, int piece, Square sq, int delta, double baselineLoss) {
    
    // File-mirror inside same color
    const Square floppedSquare = MirrorFile(sq);

    // Map to black by rank-mirroring
    const Square blackSquare = MirrorRank(sq);
    const Square blackFlopped = MirrorRank(floppedSquare);

    // Save old packed score (we need separate weights
    // even though endgame tables are symmetrical,
    // because midgame tables aren't 
    const int wOld1 = pst[White][piece][sq];
    const int wOld2 = pst[White][piece][floppedSquare];

    auto bumpEG = [&](int packed, int d) {
        return MakeScore(ScoreMG(packed), ScoreEG(packed) + d);
    };

    // Apply symmetric EG adjustments
    pst[White][piece][sq] = bumpEG(wOld1, delta);
    pst[White][piece][floppedSquare] = bumpEG(wOld2, delta);
    pst[Black][piece][blackSquare] = bumpEG(wOld1, delta);
    pst[Black][piece][blackFlopped] = bumpEG(wOld2, delta);

    const double newLoss = Tuner.TexelFit(pos);

    if (newLoss + 1e-12 < baselineLoss) {
        std::cout << "EG success: piece " << piece << " sq " << SquareName(sq)
                  << " delta " << delta << " -> loss " << newLoss
                  << " (was " << baselineLoss << ")\n";
        return newLoss; // keep
    }

    // Revert all four squares
    pst[White][piece][sq] = wOld1;
    pst[White][piece][floppedSquare] = wOld2;
    pst[Black][piece][blackSquare] = wOld1;
    pst[Black][piece][blackFlopped] = wOld2;

    std::cout << "EG no-improve: loss " << baselineLoss << "\n";
    return baselineLoss;
}

#endif