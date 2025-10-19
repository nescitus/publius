// Publius - Didactic public domain bitboard chess engine 
// by Pawel Koziol

#include "types.h"
#include "position.h"

// In the endgames with reduced material, normal
// evaluation often does not apply. Therefore we use
// some heuristics to detect when to divide score 
// to reflect increased drawing chances. We cater for 
// the most obvious 1 vs 1 or 2 vs 1 piece combos.
// Additionally, we pull the score closer to draw 
// in positions with low and almost equal material, 
// so that the engine will not go for an illusory 
// advantage in, say, rook vs rook endgame.
//
// Fully realized draw detector would recognize
// at least some rook endgames and bishop of wrong 
// color with an edge pawn.

const int certainDrawMultiplier = 0;
const int drawishMultiplier = 16;
const int noDrawMultiplier = 64;

int GetDrawMul(Position* pos, const Color strong, const Color weak) {

    // Cache counts
    const int sP = pos->Count(strong, Pawn);
    const int sN = pos->Count(strong, Knight);
    const int sB = pos->Count(strong, Bishop);
    const int sR = pos->Count(strong, Rook);
    const int sQ = pos->Count(strong, Queen);
    const int sMi = sN + sB;
    const int sMa = sR + sQ;

    const int wP = pos->Count(weak, Pawn);
    const int wN = pos->Count(weak, Knight);
    const int wB = pos->Count(weak, Bishop);
    const int wR = pos->Count(weak, Rook);
    const int wQ = pos->Count(weak, Queen);
    const int wMi = wN + wB;
    const int wMa = wR + wQ;

    // Stronger side has no pawns
    if (sP == 0) {

        // With no pawns, a single minor piece 
        // cannot win (KK, KBK, KNK, KBKP, KNKP)
        if (sMa == 0 && sMi <= 1)
            return certainDrawMultiplier;

        // KR vs Km(p)
        if (sQ == 0 && sR == 1 && sMi == 0 && wMa == 0 && wMi == 1 ) 
            return drawishMultiplier;

        // KRm vs KR(p)
        if (sQ == 0 && sR == 1 && sMi == 1 && wQ == 0 && wR == 1 && wMi == 0)
           return drawishMultiplier;

        // KQm vs KQ(p)
        if (sQ == 1 && sR == 0 && sMi == 1 && wQ == 1 && wR == 0 && wMi == 0) 
            return drawishMultiplier;

        // No pawns of either side
        if (wP == 0) {

            // No minors of either color
            if (sMi + wMi == 0) {

                // Equal count of rooks and queens -> pull towards draw,
                // as wins are achievable mainly by short term tactics
                if (sR == wR && sQ == wQ)
                    return drawishMultiplier;
            }

            // No major pieces of either color
            if (sMa + wMa == 0) {

                // Equal count of minor pieces - pull towards draw
                if (sMi == wMi)
                    return drawishMultiplier;

                // Stronger side has two minor pieces
                if (sMi == 2) {
                    
                    // Two knights will not win, alone
                    // or against a minor piece (two 
                    // knights vs a pawn is already excluded 
                    // by the earlier conditions)
                    if (sN == 2)
                        return certainDrawMultiplier;
                    
                    // Two minors vs one is generally a draw,
                    // but two bishops may win against a knight
                    if (wMi == 1 && (sB == 1 || sN > 0) )
                        return drawishMultiplier;
                    
                } // stronger side has two minors
            } // no major pieces of either color
        } // no pawns of either color
    } // no pawns od stronger side

    return noDrawMultiplier;
}