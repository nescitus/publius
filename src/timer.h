#pragma once

enum eTimeData { wTime, bTime, wIncrement, bIncrement, engTime, engInc, movesToGo, moveTime, 
	             maxDepth, maxNodes, isInfinite, timerDataSize };

struct UCItimer {
private:
    int data[timerDataSize]; // various data used to set actual time per move (see eTimeData)
    int startTime;           // when we have begun searching
    int hardTimeLimit;       // basic time allocated for a move
    int softTimeLimit;
    bool isStrict;
public:
    void Clear(void);
    void SetStartTime();
    void SetMoveTiming(void);
	int Now(void);
    int Elapsed(void);
    int IsInfiniteMode(void);
    bool ShouldFinishIteration(void);
    bool TimeHasElapsed(void);
    int GetData(const int slot);
    void SetData(const int slot, const int val);
    void SetDataForColor(const Color color);
};

extern UCItimer Timer;
