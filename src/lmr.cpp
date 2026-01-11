// Publius - Didactic public domain bitboard chess engine 
// by Pawel Koziol

#include "lmr.h"
#include "math.h"

LmrData::LmrData() {

    // Set depth of late move reduction
    for (int depth = 0; depth < 64; depth++)
        for (int moves = 0; moves < 64; moves++) {

            // log(0) is undefined, so we don't calculate it
            int r = (depth == 0 || moves == 0)
                ? 0
                : 0.38 + log(depth) * log(moves) / 2.17;

            table[0][depth][moves] = r + 1; // zero window node
            table[1][depth][moves] = r;     // principal variation node
        }
}