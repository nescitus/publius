#include "color.h"
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
EngineState State;
MoveGenerator GenerateMoves;
HistoryData History;
TransTable TT;
LmrData Lmr;
PvCollector Pv;

int main() 
{
  Mask.Init();
  Key.Init();
  Params.Init();
  GenerateMoves.Init();
  State.Init();
  UciLoop();
  TT.Exit();
  return 0;
}

void EngineState::Init() 
{
    isStopping = false;
    isPondering = false;
}
