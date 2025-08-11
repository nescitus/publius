// Publius - Didactic public domain bitboard chess engine by Pawel Koziol

#include <iomanip> // for std::setw, std::setprecision
#include "types.h"
#include "square.h"
#include "publius.h"
#include "evaldata.h"
#include "eval.h"

Square MirrorFile(Square sq) { return Square(sq ^ 7); }  // A<->H
Square MirrorRank(Square sq) { return Square(sq ^ 56); }  // 1<->8

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

    std::cout << "{\n";
    for (int r = 0; r < 8; ++r) {
        std::cout << "  ";
        for (int f = 0; f < 8; ++f) {
            Square square = MakeSquare(f, r);

            // revert initialization by substracting material value
            int v = pst[White][piece][square] - pieceValue[piece];
            int mg = ScoreMG(v);
            int eg = ScoreEG(v);
            std::cout << "S(" << std::setw(3) << mg << "," << std::setw(3) << eg << ")";
                if (r != 7 || f != 7) std::cout << ", ";
            }
            std::cout << "\n";
        }
        std::cout << "};";
}

double Parameters::TryChangeMgPst(Position *pos, int piece, Square square, int delta, double baselineLoss) { 
    
    int oldWeight = Params.pst[White][piece][square];
    int mgWeight = ScoreMG(oldWeight);
    int egWeight = ScoreEG(oldWeight);
    int mirror = InvertSquare(square);

    Params.pst[White][piece][square] = MakeScore(mgWeight + delta, egWeight);
    Params.pst[Black][piece][mirror] = MakeScore(mgWeight + delta, egWeight);

    double newLoss = Tuner.TexelFit(pos);

    if (newLoss + 1e-12 < baselineLoss) {
        std::cout << "MG success: piece " << piece << " sq " << int(sq)
                  << " delta " << delta << " -> loss " << newLoss
                  << " (was " << baselineLoss << ")\n";
        return newLoss; // keep new values
    }
    else {
        // revert
        pst[White][piece][square] = oldWeight;
        pst[Black][piece][mirror] = oldWeight;
        std::cout << "EG no-improve: loss " << baselineLoss << "\n";
        return baselineLoss;
    }
}

double Parameters::TryChangeEgPst(Position* pos, int piece, Square sq, int delta, double baselineLoss) {
    
    // File-mirror inside same color
    const Square sqF = MirrorFile(sq);

    // Map to black by rank-mirroring
    const Square bSq = MirrorRank(sq);
    const Square bSqF = MirrorRank(sqF);

    // Save old packed scores (we need all four to revert correctly)
    const int wOld1 = pst[White][piece][sq];
    const int wOld2 = pst[White][piece][sqF];
    const int bOld1 = pst[Black][piece][bSq];
    const int bOld2 = pst[Black][piece][bSqF];

    auto bumpEG = [&](int packed, int d) {
        return MakeScore(ScoreMG(packed), ScoreEG(packed) + d);
    };

    // Apply symmetric EG adjustments
    pst[White][piece][sq] = bumpEG(wOld1, delta);
    pst[White][piece][sqF] = bumpEG(wOld2, delta);
    pst[Black][piece][bSq] = bumpEG(bOld1, delta);
    pst[Black][piece][bSqF] = bumpEG(bOld2, delta);

    const double newLoss = Tuner.TexelFit(pos);

    if (newLoss + 1e-12 < baselineLoss) {
        std::cout << "EG success: piece " << piece << " sq " << int(sq)
                  << " delta " << delta << " -> loss " << newLoss
                  << " (was " << baselineLoss << ")\n";
        return newLoss; // keep
    }

    // Revert all four squares
    pst[White][piece][sq] = wOld1;
    pst[White][piece][sqF] = wOld2;
    pst[Black][piece][bSq] = bOld1;
    pst[Black][piece][bSqF] = bOld2;

    std::cout << "EG no-improve: loss " << baselineLoss << "\n";
    return baselineLoss;
}

#endif