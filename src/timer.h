// Publius - Didactic public domain bitboard chess engine by Pawel Koziol

#pragma once

enum eTimeData { wTime, bTime, wIncrement, bIncrement, engTime, engInc, movesToGo, moveTime, 
                 maxDepth, maxNodes, isInfinite, timerDataSize };

struct UCItimer {
private:
    size_t data[timerDataSize]; // various data used to set actual time per move (see eTimeData)
    size_t startTime;           // when we have begun searching
    size_t hardTimeLimit;       // basic time allocated for a move
    size_t softTimeLimit;       // but we won't start the next iteration after exceeding this
    bool isStrict;
    bool isRepeating;        // repeating TC uses strict mode (does it help?)...
public:
    size_t nodeCount;      // counter of visited nodes
    bool waitingForStop;
    bool isStopping;
    bool isPondering;
    void SetRepeating(void); //...and we won't detect it just by checking movestogo
    void Clear(void);
    void SetStartTime();
    void SetMoveTiming(void);
    size_t Now(void);
    size_t Elapsed(void);
    int IsInfiniteMode(void);
    bool ShouldFinishIteration(void);
    bool TimeHasElapsed(void);
    size_t GetData(const int slot);
    void SetData(const int slot, const size_t val);
    void SetDataForColor(const Color color);
    bool IsTimeout(void);
    void TryStoppingByTimeout(void);
    void TryStoppingByNodecount(void);
};

extern UCItimer Timer;
