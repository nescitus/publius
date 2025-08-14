// Publius - Didactic public domain bitboard chess engine by Pawel Koziol

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

    // Stronger side has no pawns
    if (pos->Count(strong, Pawn) == 0) {

        // With no pawns, a single minor piece 
        // cannot win (KK, KBK, KNK, KBKP, KNKP)
        if (pos->CountMajors(strong) == 0 &&
            pos->CountMinors(strong) <= 1)
            return certainDrawMultiplier;

        // KR vs Km(p)
        if (pos->Count(strong, Queen) == 0 &&
            pos->Count(strong, Rook) == 1 &&
            pos->CountMinors(strong) == 0 &&
            pos->CountMajors(weak) == 0 &&
            pos->CountMinors(weak) == 1
            ) return drawishMultiplier;

        // KRm vs KR(p)
        if (pos->Count(strong, Queen) == 0 &&
            pos->Count(strong, Rook) == 1 &&
            pos->CountMinors(strong) == 1 &&
            pos->Count(weak, Queen) == 0 &&
            pos->Count(weak, Rook) == 1 &&
            pos->CountMinors(weak) == 0
            ) return drawishMultiplier;

        // KQm vs KQ(p)
        if (pos->Count(strong, Queen) == 1 &&
            pos->Count(strong, Rook) == 0 &&
            pos->CountMinors(strong) == 1 &&
            pos->Count(weak, Queen) == 1 &&
            pos->Count(weak, Rook) == 0 &&
            pos->CountMinors(weak) == 0 
            ) return drawishMultiplier;

        // No pawns of either side
        if (pos->Count(weak, Pawn) == 0) {

            // No minors of either color
            if (pos->CountMinors(strong) + pos->CountMinors(weak) == 0) {

                // Equal count of rooks and queens -> pull towards draw,
                // as wins are achievable mainly by short term tactics
                if (pos->Count(strong, Rook) == pos->Count(weak, Rook) &&
                    pos->Count(strong, Queen) == pos->Count(weak, Queen))
                    return drawishMultiplier;
            }

            // No major pieces of either color
            if (pos->CountMajors(strong) + pos->CountMajors(weak) == 0) {

                // Equal count of minor pieces - pull towards draw
                if (pos->CountMinors(strong) == pos->CountMinors(weak))
                    return drawishMultiplier;

                // Stronger side has two minor pieces
                if (pos->CountMinors(strong) == 2) {
                    
                    // Two knights will not win,
                    // alone or against a minor piece
                    // (note that two knights vs a pawn
                    // is already excluded by the earlier 
                    // conditions)
                    if (pos->Count(strong, Knight) == 2)
                        return certainDrawMultiplier;
                    
                    // Two minors vs one is generally a draw,
                    // but two bishops may win against a knight
                    if (pos->CountMinors(weak) == 1) {
                        if (pos->Count(weak, Bishop) == 1 ||
                            pos->Count(strong, Knight) > 0)
                            return drawishMultiplier;
                    }
                } // stronger side has two minors
            } // no major pieces of either color
        } // no pawns of either color
    } // no pawns od stronger side

    return noDrawMultiplier;
}