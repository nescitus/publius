// Publius - Didactic public domain bitboard chess engine 
// by Pawel Koziol

#include <chrono>
#include <stdio.h>
#include "types.h"
#include "limits.h"
#include "position.h"
#include "timer.h"

#define NOMINMAX

void UCItimer::Clear(void) {

    isStopping = false;
    isPondering = false;
    waitingForStop = false;
    SetData(maxDepth, 64);
    hardTimeLimit = 0;
    softTimeLimit = 0;
    SetData(wTime, 0);
    SetData(bTime, 0);
    SetData(wIncrement, 0);
    SetData(bIncrement, 0);
    SetData(moveTime, 0);
    SetData(maxNodes, 0);
    SetData(movesToGo, 30);
    SetData(isInfinite, 0);
    isStrict = false;
    isRepeating = false;
}

void UCItimer::RefreshStats() {
    timeUsed = Elapsed();
    nps = timeUsed ? (nodeCount * 1000 / timeUsed) : 0;
}

void UCItimer::SetRepeating() {
    isRepeating = true;
}

void UCItimer::SetStartTime(void) {
    startTime = Now();
}

void UCItimer::SetMoveTiming(void) {

    // Certain settings forbid using tricks to finish earlier
    isStrict = (data[maxDepth] < 64 || // depth limit set
                data[maxNodes] > 0 ||  // node limit set
                data[moveTime]);       //||  // time per move set
     //   isRepeating)                 // repeating time control

    // TODO: test whether repeating time control
    // benefits from strict mode (I hope not)

    // User-defined time per move
    if (data[moveTime]) {
        hardTimeLimit = data[moveTime];
        softTimeLimit = data[moveTime];
        return;
    }

    // We set two time limits: hardTimeLimit (break
    // the search no matter what) and softTimeLimit
    // (don't start the next iteration). There is
    // some scope for smarter tricks taking into
    // account root move changes and fail lows, 
    // but our current implementation focuses
    // on staying out of trouble: counteracting 
    // the GUI lag and being extra careful under 
    // the incremental time control near the end 
    // of the game.

    if (data[engTime] >= 0) {

        // safeguard for the last move of repeating time control
        if (data[movesToGo] == 1)
            data[engTime] -= std::min((size_t)1000, data[engTime] / 10);

        // calculate move time
        hardTimeLimit = (data[engTime] + data[engInc] * (data[movesToGo] - 1)) / data[movesToGo];

        // while in time trouble, try to save a bit on increment
        if (hardTimeLimit < data[engInc])
            hardTimeLimit -= ((data[engInc] * 4) / 5);

        // ensure that our limit does not exceed total time available
        if (hardTimeLimit > data[engTime])
            hardTimeLimit = data[engTime];

        // safeguard against a lag
        hardTimeLimit -= 10;

        // calculate soft time limit,
        // used to determine when to finish iteration
        softTimeLimit = hardTimeLimit / 2;

        // ensure that we have non-zero time
        if (hardTimeLimit < 1) {
            hardTimeLimit = 1;
            softTimeLimit = 1;
        }
    }
}

size_t UCItimer::Now(void) {

    using namespace std::chrono;
    return duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
}

void UCItimer::TryStoppingByTimeout() {
    if (IsTimeout())
        isStopping = true;
}

void UCItimer::TryStoppingByNodecount() {

    if (GetData(maxNodes) != 0 && !IsInfiniteMode()) {
        if (nodeCount >= GetData(maxNodes))
            isStopping = true;
    }
}

bool UCItimer::IsTimeout() {
    return (!isPondering && !IsInfiniteMode() && TimeHasElapsed());
}

bool UCItimer::TimeHasElapsed(void) {

    if (IsInfiniteMode())
        return false;

    return (Elapsed() >= hardTimeLimit);
}

size_t UCItimer::Elapsed(void) {
    return (Now() - startTime);
}

int UCItimer::IsInfiniteMode(void) {
    return(data[isInfinite]);
}

bool UCItimer::ShouldFinishIteration(void) {
    
    // faster timeout not applicable
    if (isStrict || IsInfiniteMode() ) 
        return false;
    
    return (Elapsed() >= softTimeLimit);
}

size_t UCItimer::GetData(const int slot) {
    return data[slot];
}

void UCItimer::SetData(const int slot, const size_t val) {
    data[slot] = val;
}

void UCItimer::SetDataForColor(const Color color) {
    data[engTime] = color == White ? GetData(wTime) : GetData(bTime);
    data[engInc] =  color == White ? GetData(wIncrement) : GetData(bIncrement);
}