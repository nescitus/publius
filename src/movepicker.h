#pragma once

enum {stageTT, stageGenCapt, stageReturnGoodCapt, stageGenQuiet, stageReturnQuiet, stageReturnBad};

class MovePicker {
private:
    int allCaptLength, goodCaptureLength, badCaptureLength, quietLength;
    int allCaptCnt, goodCaptureCnt, badCaptureCnt, quietCnt;
    MoveList allCaptureList, goodCaptureList, badCaptureList, quietList;
public:
    Move moveFromTT;
    int stage;
    void InitAllMoves(Move ttMove);
    Move NextMove(Position* pos, int ply);
};