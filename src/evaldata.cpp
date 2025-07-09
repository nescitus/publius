#include "types.h"
#include "evaldata.h"


void EvalData::Clear() {

    phase = 0;
    
    for (Color color = White; color < colorNone; ++color) {
        
        // Clean partial scores
        mg[color] = 0;
        eg[color] = 0;
        mgPawn[color] = 0;
        egPawn[color] = 0;

        // Clear king attacker counters
        minorAttacks[color] = 0;
        rookAttacks[color] = 0;
        queenAttacks[color] = 0;

        // Clear all ttacks bitboard
        allAtt[color] = 0;
        
        // Clear control maps
        for (int piece = 0; piece < 6; piece++)
            control[color][piece] = 0;
    }
}

// Add score
void EvalData::Add(Color color, int mgVal, int egVal) {
    mg[color] += mgVal;
    eg[color] += egVal;
}

// Add pawn score. Pawn scores are handled separately,
// so that they can be saved in pawn hashtable.
void EvalData::AddPawn(Color color, int mgVal, int egVal) {
    mgPawn[color] += mgVal;
    egPawn[color] += egVal;
}