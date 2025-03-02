#include "lmr.h"
#include "math.h"

void LmrData::Init() {

    // Set depth of late move reduction

    for (int depth = 0; depth < 64; depth++)
        for (int moves = 0; moves < 64; moves++) {

            int r = log(depth) * log(moves) / 2;
            if (depth == 0 || moves == 0) r = 0; // otherwise undefined

            table[0][depth][moves] = r + 1; // zero window node
            table[1][depth][moves] = r;     // principal variation node
        }
}