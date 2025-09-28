// Publius - Didactic public domain bitboard chess engine by Pawel Koziol

#include <iomanip> // for std::setw, std::setprecision
#include <iostream> // cout
#include "types.h"
#include "square.h"
#include "position.h" // for tuning
#include "score.h"
#include "publius.h" // USE_TUNING
#include "params.h"
#include "tuner.h"

void Parameters::Init(void) {

    for (Color color = White; color < colorNone; ++color) {
        for (Square square = A1; square < sqNone; ++square) {

            pawnSupport[color][RelativeSq(color, square)] = S(p_support[square],0);

            for (PieceType piece = Pawn; piece < noPieceType; ++piece)
                pst[color][piece][RelativeSq(color, square)] = pieceValue[piece] + allPst[piece][square];
        }
    }
}

void Parameters::TunePst() {

#ifdef USE_TUNING
    Position pos;

    // calculate initial fit
    Tuner.Init(0);
    double initialFit = Tuner.TexelFit(&pos);
    std::cout << "Initial fit is " << initialFit << std::endl;

    // Create a sample. We don't tune using
    // the entire dataset, as it would take
    // way too much time. 
    Tuner.Init(1000);
    double currentFit = Tuner.TexelFit(&pos);

    int delta = 1;
    // possible improvement: randomize order
    for (Square s = A1; s <= H8; ++s) {
        currentFit = Tuner.TuneSingleSquare(&pos, this, Pawn, s, delta, currentFit);
        currentFit = Tuner.TuneSingleSquare(&pos, this, Knight, s, delta, currentFit);
        currentFit = Tuner.TuneSingleSquare(&pos, this, Bishop, s, delta, currentFit);
        currentFit = Tuner.TuneSingleSquare(&pos, this, Rook, s, delta, currentFit);
        currentFit = Tuner.TuneSingleSquare(&pos, this, Queen, s, delta, currentFit);
        currentFit = Tuner.TuneSingleSquare(&pos, this, King, s, delta, currentFit);
    }

    Tuner.Init(0);
    double finalFit = Tuner.TexelFit(&pos);

    std::cout << "final fit " << finalFit << " (was " << initialFit << ")" << std::endl;

    // we have succeeded, here are the new tables
    if (finalFit < initialFit)
        PrintAll();
    else
        std::cout << "Tuning session failed!";
#endif
}

// Prints all the piece/square tables, so that
// they can be pasted directly into the code
void Parameters::PrintAll() {

    std::cout << "const int allPst[6][64] = {\n";
    for (PieceType piece = Pawn; piece < noPieceType; ++piece) {
        PrintPst(piece);
        if (piece != King) std::cout << ",";
    }
    std::cout << "};\n";
}

// Prints a single piece/square table 
// (which is a part of a larger table)
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
    std::cout << "}";
}