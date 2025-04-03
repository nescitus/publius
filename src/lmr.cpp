#include "lmr.h"
#include "math.h"

LmrData::LmrData() {

    int r;

    // Set depth of late move reduction
    for (int depth = 0; depth < 64; depth++)
        for (int moves = 0; moves < 64; moves++) {

            if (depth == 0 || moves == 0)
                r = 0; // log(0) is undefined
            else
                r = log(depth) * log(moves) / 2;

            table[0][depth][moves] = r + 1; // zero window node
            table[1][depth][moves] = r;     // principal variation node
        }
}