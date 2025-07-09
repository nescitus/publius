#include "types.h"
#include "square.h"
#include "limits.h"
#include "publius.h"
#include "timer.h"
#include "uci.h"
#include "mask.h"
#include "hashkeys.h"
#include "history.h"
#include "trans.h"
#include "bitgen.h"
#include "lmr.h"
#include "pv.h"
#include <stdio.h>

UCItimer Timer;
MaskData Mask;
HashKeys Key;
Parameters Params;
MoveGenerator GenerateMoves;
HistoryData History;
TransTable TT;
LmrData Lmr;
PvCollector Pv;

int main()
{
    Params.Init();
    GenerateMoves.Init();
    Timer.Clear(); // clears also search statis flags
    UciLoop();
    TT.Exit();
    return 0;
}
