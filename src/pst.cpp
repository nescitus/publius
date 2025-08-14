// Publius - Didactic public domain bitboard chess engine by Pawel Koziol

#include <iomanip> // for std::setw, std::setprecision
#include "types.h"
#include "square.h"
#include "position.h"
#include "score.h"
#include "evaldata.h"
#include "eval.h" // values
#include "publius.h" // USE_TUNING
#include "params.h"
#include "tuner.h"

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
    /**/
    PrintPst(Pawn);
    Position pos;
    Tuner.Init(4000);
    double currentFit = Tuner.TexelFit(&pos);
    
    for (Square s = A2; s <= H7; ++s) 
        currentFit = Tuner.TuneSingleSquare(&pos, this, Pawn, s,  currentFit);
    
    PrintPst(Pawn);
    /**/
#endif
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