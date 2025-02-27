#include "color.h"
#include "piece.h"
#include "square.h"
#include "publius.h"

int GetDrawMul(Position* pos, Color strong, Color weak) {

    // stronger side has no pawns
    if (pos->GetCount(strong, Pawn) == 0) {

        // With no pawns, a single minor piece 
        // cannot win (KK, KBK, KNK, KBKP, KNKP)

        if (pos->GetMajorCount(strong) == 0
            && pos->GetMinorCount(strong) <= 1)
            return 0;

        // KR vs Km(p)

        if (pos->GetCount(strong, Queen) == 0
            && pos->GetCount(strong, Rook) == 1
            && pos->GetMinorCount(strong) == 0
            && pos->GetMajorCount(weak) == 0
            && pos->GetMinorCount(weak) == 1
            ) return 16;

        // KRm vs KR(p)

        if (pos->GetCount(strong, Queen) == 0
            && pos->GetCount(strong, Rook) == 1
            && pos->GetMinorCount(strong) == 1
            && pos->GetCount(weak, Queen) == 0
            && pos->GetCount(weak, Rook) == 1
            && pos->GetMinorCount(weak) == 0
            ) return 16;

        // KQm vs KQ(p)

        if (pos->GetCount(strong, Queen) == 1
            && pos->GetCount(strong, Rook) == 0
            && pos->GetMinorCount(strong) == 1
            && pos->GetCount(weak, Queen) == 1
            && pos->GetCount(weak, Rook) == 0
            && pos->GetMinorCount(weak) == 0
            ) return 16;
    }

    return 64;
}