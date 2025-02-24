#pragma once

enum eTimeData { wTime, bTime, wInc, bInc, engTime, engInc, movesToGo, moveTime, 
	             maxDepth, isInfinite, timerDataSize };

struct sTimer {
private:
    int data[timerDataSize]; // various data used to set actual time per move (see eTimeData)
    int startTime;          // when we have begun searching
    int timeForMove;        // basic time allocated for a move
public:
    void Clear(void);
    void SetStartTime();
    void SetMoveTiming(void);
	int Now(void);
    int Elapsed(void);
    int IsInfiniteMode(void);
    int TimeHasElapsed(void);
    int GetData(int slot);
    void SetData(int slot, int val);
    void SetSideData(Color side);
};

extern sTimer Timer;
