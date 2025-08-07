// Publius - Didactic public domain bitboard chess engine by Pawel Koziol

#include "types.h"
#include "bitboard.h"
#include "evaldata.h"

void EvalData::Clear() {

    phase = 0;
    
    for (Color color = White; color < colorNone; ++color) {
        
        // Clean scores
        score[color] = 0;
        pawnScore[color] = 0;

        // Clear king attack units counter
        kingAttUnits[color] = 0;

        // Clear all attacks bitboard
        allAtt[color] = 0;
        
        // Clear control maps
        for (int pieceType = 0; pieceType < 6; pieceType++)
            control[color][pieceType] = 0;
    }
}

// Add
void EvalData::Add(Color color, int val) {
    score[color] += val;
}

// Add pawn score. It is handled separately,
// so that it can be saved in pawn hashtable.
void EvalData::AddPawn(Color color, int val) {
    pawnScore[color] += val;
}

void EvalData::AddAttacks(Color color, Bitboard att, int strong, int weak) {
    
    if (att) {
        kingAttUnits[color] += strong * PopCnt(att & ~control[~color][Pawn]);
        kingAttUnits[color] += weak * PopCnt(att & control[~color][Pawn]);
    }
}