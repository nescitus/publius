// Publius - Didactic public domain bitboard chess engine 
// by Pawel Koziol

#include "types.h"
#include "limits.h"
#include "position.h"
#include "timer.h"
#include "uci.h"
#include "mask.h"
#include "hashkeys.h"
#include "history.h"
#include "trans.h"
#include "bitgen.h"
#include "lmr.h"
#include "pv.h"
#include "score.h"
#include "params.h"
#include "publius.h"
#include "nn.h"

UCItimer Timer;
MaskData Mask;
HashKeys Key;
Parameters Params;
MoveGenerator GenerateMoves;
HistoryData History;
TransTable TT;
LmrData Lmr;
PvCollector Pv;
Net NN;
bool isNNUEloaded;

int main() {

    Params.Init();
    TryLoadingNNUE("publius_net32_1.bin");
    GenerateMoves.Init();
    UciLoop();
    TT.Exit();
    return 0;
}