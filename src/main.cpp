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
#include "util.h"

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
bool isUci;
int nnueWeight;
int hceWeight;

int main() {

#ifdef USE_TUNING
    std::cout << "This version uses tuning and can win by memory hogging\n"
              << "Comment out USE_TUNING in publius.h and recompile\n";
#endif

    isUci = false;
    Params.Init();
    nnueWeight = 80;
    hceWeight = 0;
    TryLoadingNNUE(netPath);
    GenerateMoves.Init();
    UciLoop();
    TT.Exit();
    return 0;
}