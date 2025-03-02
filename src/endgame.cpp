#include "color.h"
#include "piece.h"
#include "square.h"
#include "limits.h"
#include "publius.h"

int GetDrawMul(Position* pos, Color strong, Color weak) {

    // stronger side has no pawns
    if (pos->Count(strong, Pawn) == 0) {

        // With no pawns, a single minor piece 
        // cannot win (KK, KBK, KNK, KBKP, KNKP)

        if (pos->CountMajors(strong) == 0
            && pos->CountMinors(strong) <= 1)
            return 0;

        // KR vs Km(p)

        if (pos->Count(strong, Queen) == 0
            && pos->Count(strong, Rook) == 1
            && pos->CountMinors(strong) == 0
            && pos->CountMajors(weak) == 0
            && pos->CountMinors(weak) == 1
            ) return 16;

        // KRm vs KR(p)

        if (pos->Count(strong, Queen) == 0
            && pos->Count(strong, Rook) == 1
            && pos->CountMinors(strong) == 1
            && pos->Count(weak, Queen) == 0
            && pos->Count(weak, Rook) == 1
            && pos->CountMinors(weak) == 0
            ) return 16;

        // KQm vs KQ(p)

        if (pos->Count(strong, Queen) == 1
            && pos->Count(strong, Rook) == 0
            && pos->CountMinors(strong) == 1
            && pos->Count(weak, Queen) == 1
            && pos->Count(weak, Rook) == 0
            && pos->CountMinors(weak) == 0
            ) return 16;
    }

    return 64;
}