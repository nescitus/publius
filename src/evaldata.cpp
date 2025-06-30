#include "types.h"
#include "evaldata.h"


void EvalData::Clear() {

    phase = 0;
    
    for (Color color = White; color < colorNone; ++color) {
        mg[color] = 0;
        eg[color] = 0;
        mgPawn[color] = 0;
        egPawn[color] = 0;
        minorAttacks[color] = 0;
        rookAttacks[color] = 0;
        queenAttacks[color] = 0;
        allAtt[color] = 0;
        
        for (int piece = 0; piece < 6; piece++) {
            control[color][piece] = 0;
        }
    }
}

void EvalData::Add(Color color, int mgVal, int egVal) {
	mg[color] += mgVal;
	eg[color] += egVal;
}

void EvalData::AddPawn(Color color, int mgVal, int egVal) {
    mgPawn[color] += mgVal;
    egPawn[color] += egVal;
}