// Publius - Didactic public domain bitboard chess engine by Pawel Koziol

#include "types.h"
#include "limits.h"
#include "publius.h"
#include "hashkeys.h"
#include "move.h"
#include "corrhist.h"

// -4 Elo after 1000 games

const int arraySize = 16384;
const int maxBonus = 256;
const int minBonus = -256;

// clear all values
void CorrHist::Clear(void) {

    for (int i = 0; i < 16384; i++)
        pawnCorrhist[i] = 0;
}

void CorrHist::Update(Position *pos, int score, int eval, int depth)
{
    int key = GetPawnKey(pos);
    int delta = std::clamp((score - eval) * depth / 8, minBonus, maxBonus);
    pawnCorrhist[key] += (short)(delta - pawnCorrhist[key] * std::abs(delta) / 1024);
}

int CorrHist::Correct(Position* pos, int eval) {

    int key = GetPawnKey(pos);
    int correction = 16 * pawnCorrhist[key];
    correction /= 1024;
    return  std::clamp(eval + correction, -29999, 29999);
}

int CorrHist::GetPawnKey(Position* pos) {

    Bitboard key = pos->pawnHash;
    key ^= Key.ForPiece(White, King, pos->KingSq(White));
    key ^= Key.ForPiece(Black, King, pos->KingSq(Black));
    return (int)(key % arraySize);
}