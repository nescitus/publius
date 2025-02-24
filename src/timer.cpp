#include <stdio.h>
#include "publius.h"
#include "timer.h"

#define NOMINMAX

#if defined(_WIN32) || defined(_WIN64)
#  include <windows.h>
#else
#  include <unistd.h>
#  include <sys/time.h>
#endif

void sTimer::Clear(void) {
    SetData(maxDepth, 64);
    timeForMove = -1;
    SetData(wTime, -1);
    SetData(bTime, -1);
    SetData(wInc, 0);
    SetData(bInc, 0);
    SetData(moveTime, 0);
    SetData(movesToGo, 40);
    SetData(isInfinite, 0);
}

void sTimer::SetStartTime(void) {
    startTime = Now();
}

void sTimer::SetMoveTiming(void) {

    // User-defined time per move, no tricks available

    if (data[moveTime]) {
        timeForMove = data[moveTime];
        return;
    }

    // We are operating within some time limit. There is some scope for using
    // remaining  time  in a clever way, but current  implementation  focuses
    // on staying out of trouble: counteracting the GUI lag and being careful
    // under the incremental time control near the end of the game.

    if (data[engTime] >= 0) {

        if (data[movesToGo] == 1) {
            data[engTime] -= std::min(1000, data[engTime] / 10);
        }

        timeForMove = (data[engTime] + data[engInc] * (data[movesToGo] - 1)) / data[movesToGo];

        // while in time trouble, try to save a bit on increment
        if (timeForMove < data[engInc]) {
            timeForMove -= ((data[engInc] * 4) / 5);
        }

        // ensure that our limit does not exceed total time available
        if (timeForMove > data[engTime]) {
            timeForMove = data[engTime];
        }

        // safeguard against a lag
        timeForMove -= 10;

        // ensure that we have non-zero time
        if (timeForMove < 1) timeForMove = 1;
    }
}

int sTimer::Now(void) {

#if defined(_WIN32) || defined(_WIN64)
#if _WIN32_WINNT >= 0x0600 // Windows Vista or later
    return GetTickCount64();
#else
    return GetTickCount();
#endif
#else
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
#endif
}

int sTimer::Elapsed(void) {
    return (Now() - startTime);
}

int sTimer::IsInfiniteMode(void) {
    return(data[isInfinite]);
}

int sTimer::TimeHasElapsed(void) {
    return (Elapsed() >= timeForMove);
}

int sTimer::GetData(int slot) {
    return data[slot];
}

void sTimer::SetData(int slot, int val) {
    data[slot] = val;
}

void sTimer::SetSideData(Color side) {
    data[engTime] = (bool)(side == White) ? GetData(wTime) : GetData(bTime);
    data[engInc] =  (bool)(side == White) ? GetData(wInc) : GetData(bInc);
}