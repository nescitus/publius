#include "color.h"
#include "square.h"
#include "limits.h"
#include "publius.h"

void EvalData::Clear() {
    phase = 0;
    for (Color color = White; color < colorNone; ++color) {
        mg[color] = 0;
        eg[color] = 0;
        minorAttacks[color] = 0;
        rookAttacks[color] = 0;
        queenAttacks[color] = 0;
    }
}

void EvalData::Add(Color color, int mgVal, int egVal) {
	mg[color] += mgVal;
	eg[color] += egVal;
}