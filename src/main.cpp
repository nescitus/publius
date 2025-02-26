#include "color.h"
#include "square.h"
#include "publius.h"
#include "timer.h"
#include "uci.h"
#include "mask.h"
#include "hashdata.h"
#include "history.h"
#include "trans.h"
#include "bitgen.h"
#include <stdio.h>

UCItimer Timer;
MaskData Mask;
HashData Key;
EngineState State;
MoveGenerator GenerateMoves;
HistoryData History;
TransTable TT;

int main() 
{
  Mask.Init();
  Key.Init();
  GenerateMoves.Init();
  InitLmr();
  InitPst();
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
