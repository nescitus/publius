// Publius - Didactic public domain bitboard chess engine 
// by Pawel Koziol

// Timer class determines when to stop the search.
// We are using both hard time limit (stop immediately)
// and soft time limit (don't start another iteration,
// as it would be probably wasteful). It might be 
// worthwhile to include "panic mode" (position is
// suddenly deteriorating, we need to look how to
// avoid the loss).

#include <chrono>
#include <stdio.h>
#include "types.h"
#include "limits.h"
#include "position.h"
#include "timer.h"

#define NOMINMAX

// reset timer data
void UCItimer::Clear(void) {

    isStopping = false;
    isPondering = false;
    waitingForStop = false;
    SetData(maxDepth, 64);
    hardLimit = 0;
    softLimit = 0;
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

// refresh statistics
void UCItimer::RefreshStats() {
    timeUsed = Elapsed();
    nps = timeUsed ? (nodeCount * 1000 / timeUsed) : 0;
}

// set repeating time control
void UCItimer::SetRepeating() {
    isRepeating = true;
}

// start the timer (remember start time, reset stats)
void UCItimer::Start(void) {

    startTime = Now();
    nodeCount = 0;
    isStopping = false;
}

// sets time for move (both soft and hard limit)
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
        hardLimit = data[moveTime];
        softLimit = data[moveTime];
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

    if (ShouldCalculateTimeControl()) {

        // safeguard for the last move of repeating time control
        if (data[movesToGo] == 1)
            data[engTime] -= std::min((size_t)1000, data[engTime] / 10);

        // calculate move time
        hardLimit = (data[engTime] + data[engInc] * (data[movesToGo] - 1)) / data[movesToGo];

        // while in time trouble, try to save a bit 
        // on increment
        if (hardLimit < 2 * data[engInc]) {
            if (hardLimit > (data[engInc] * 4) / 5)
               hardLimit -= ((data[engInc] * 4) / 5);
            else
               hardLimit = 1;
        }

        // ensure that our limit does not exceed 
        // total time available
        if (hardLimit > data[engTime])
            hardLimit = data[engTime];

        // safeguard against a lag, not allowing
        // underflow (variable is size_t, so unsigned)
        hardLimit = (hardLimit > 10) ? (hardLimit - 10) : 1;

        // calculate soft time limit,
        // used to determine when to finish iteration
        softLimit = hardLimit / 2;

        // ensure that we have non-zero time
        if (hardLimit < 1) {
            hardLimit = 1;
            softLimit = 1;
        }
    }
}

// are we expected to calculate the time limit?
bool UCItimer::ShouldCalculateTimeControl(void) {
    return (GetData(wTime) || GetData(bTime) ||
            GetData(wIncrement) || GetData(bIncrement));
}

// detects whether it makes sense to start
// a new iteration (soft limit)
bool UCItimer::ShouldFinishIteration(void) {

    // faster timeout not applicable
    if (isStrict || IsInfiniteMode())
        return false;

    return (Elapsed() >= softLimit);
}

// detects whether we should stop searching
// based on reaching the node limit
void UCItimer::TryStoppingByNodecount() {

    if (GetData(maxNodes) != 0 && !IsInfiniteMode()) {
        if (nodeCount >= GetData(maxNodes))
            isStopping = true;
    }
}

// detects whether we should stop searching
// based on timeout
void UCItimer::TryStoppingByTimeout() {
    if (IsTimeout())
        isStopping = true;
}

// detects a timeout
bool UCItimer::IsTimeout() {
    return (!isPondering && !IsInfiniteMode() && TimeHasElapsed());
}

// checks whether hard time limit has been reached
bool UCItimer::TimeHasElapsed(void) {

    if (IsInfiniteMode())
        return false;

    return (Elapsed() >= hardLimit);
}

// gets elapsed time
size_t UCItimer::Elapsed(void) {
    return (Now() - startTime);
}

// gives current time in milliseconds
size_t UCItimer::Now(void) {
    using namespace std::chrono;
    return duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
}

// checks if we are in the infinite mode
int UCItimer::IsInfiniteMode(void) {
    return(data[isInfinite]);
}

// sets time and increment for a player of given color
void UCItimer::SetDataForColor(const Color color) {
    data[engTime] = color == White ? GetData(wTime) : GetData(bTime);
    data[engInc] = color == White ? GetData(wIncrement) : GetData(bIncrement);
}

// sets information about time control conditions
void UCItimer::SetData(const int slot, const size_t val) {
    data[slot] = val;
}

// gets information about time control conditions
size_t UCItimer::GetData(const int slot) {
    return data[slot];
}