#include "publius.h"
#include "timer.h"
#include <stdio.h>

sTimer Timer;
cMask Mask;
EngineState State;
MoveGenerator GenerateMoves;
cHistory History;
TransTable TT;

int main() 
{
  Mask.Init();
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
