// Publius - Didactic public domain bitboard chess engine by Pawel Koziol

// Correction history (corrhist in short) is a kind
// of imperfect hash table, recording differences
// between static evaluation and search result.
// The difference is then used to adjust evaluation.
// Moreover, we are not using a complete hash (which
// would be redundant with transposition table and
// using it to adjust evaluation), but partial hash
// keys describing certain aspects of position.
// Corrhist logic is a bit fuzzy, because we do not
// guard against hash collisions.

// Currently we are using pawn corrhist only.

#include "types.h"
#include "limits.h"
#include "publius.h"
#include "hashkeys.h"
#include "move.h"
#include "corrhist.h"

// -3 Elo after 1000 games

const int maxBonus = 256;
const int minBonus = -256;

// clear all values
void CorrHist::Clear(void) {

    for (int i = 0; i < corrhistSize; i++) {
        pawnCorrhist[White][i] = 0;
        pawnCorrhist[Black][i] = 0;
    }
}

void CorrHist::Update(Position *pos, int score, int eval, int depth)
{
    int stm = pos->GetSideToMove();
    int key = GetPawnKey(pos);
    int delta = std::clamp((score - eval) * depth / 8, minBonus, maxBonus);
    pawnCorrhist[stm][key] += (short)(delta - pawnCorrhist[stm][key] * std::abs(delta) / 1024);
}

int CorrHist::Correct(Position* pos, int eval) {

    int stm = pos->GetSideToMove();
    int key = GetPawnKey(pos);
    int correction = 16 * pawnCorrhist[stm][key];
    correction /= 1024;
    return  std::clamp(eval + correction, -EvalLimit, EvalLimit);
}

int CorrHist::GetPawnKey(Position* pos) {

    Bitboard key = pos->pawnKingHash;
    
    // erase kings, we want pawn-only hash key
    key ^= Key.ForPiece(White, King, pos->KingSq(White));
    key ^= Key.ForPiece(Black, King, pos->KingSq(Black));

    return (int)(key % corrhistSize);
}