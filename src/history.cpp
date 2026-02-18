// Publius - Didactic public domain bitboard chess engine
// by Pawel Koziol

#include <cstdint>
#include <algorithm> // std::clamp
#include "types.h"
#include "limits.h"
#include "position.h"
#include "move.h"
#include "history.h"

// Buckets are used to compress refutation table;
// instead of [squareFrom][squareTo] we will use
// [bucketOfSquareFrom] [squareTo]
const int bucket[64] = {
    0,  0,  1,  1,  2,  2,  3,  3,
    0,  0,  1,  1,  2,  2,  3,  3,
    4,  4,  5,  5,  6,  6,  7,  7,
    4,  4,  5,  5,  6,  6,  7,  7,
    8,  8,  9,  9, 10, 10, 11, 11,
    8,  8,  9,  9, 10, 10, 11, 11,
   12, 12, 13, 13, 14, 14, 15, 15,
   12, 12, 13, 13, 14, 14, 15, 15
};

static constexpr int maxHist = 16384;
static constexpr int refCount = 16 * 64;   // 1024 real contexts
static constexpr int refNull = refCount;   // 1024 = dedicated null/none slot

// returns index of a refutation move, based on 
// bucket of from square [0..15] and to square [0..63]
static inline int RefIndex(Move refuted) {
    
    if (!refuted)
        return refNull; // 1024 (null / none)

    const int rf = bucket[GetFromSquare(refuted)]; // 0..15
    const int rt = int(GetToSquare(refuted));      // 0..63

    return rf * 64 + rt; // 0..1023
}

// HistKey struct contains data for indexing history tables
struct HistKey {
    Color side;
    int refIndex;
    Square from;
    int fromBucket;
    Square to;
    ColoredPiece piece;
};

// Initialize data for indexing the history tables
static inline HistKey MakeHistKey(Position* pos, Move move, Move refuted) {
    HistKey k;
    k.side = pos->GetSideToMove();
    k.refIndex = RefIndex(refuted);
    k.from = GetFromSquare(move);
    k.fromBucket = bucket[k.from];
    k.to = GetToSquare(move);
    k.piece = pos->GetPiece(k.from);
    return k;
}

// Constructor
HistoryData::HistoryData() {
    ClearOnNewGame();
}

// Clears all the history data
void HistoryData::ClearOnNewGame() {
    ClearOnNewSearch();                             // cutoffHistory, killers
    std::memset(refutation, 0, sizeof(refutation)); // refutation table
}

// ClearOnNewSearch() resets cutoff history 
// and killer moves, refutation table stays as is.
 void HistoryData::ClearOnNewSearch(void) {

    std::memset(cutoffHistory, 0, sizeof(cutoffHistory));

    for (int ply = 0; ply < SearchTreeSize; ply++)
        killer1[ply] = killer2[ply] = 0;
}

// Update history values on a positive outcome, like a beta cutoff.
// Returns true if we actually updated quiet history (so caller may decide
// whether to penalize other quiet tries).
bool HistoryData::Update(Position* pos, const Move move, const Move refuted, const int depth, const int ply) {

    // History is updated only for quiet moves
    if (IsMoveNoisy(pos, move))
        return false;

    // Update killer moves, taking care that they do not  repeat;
    // killer1 is always replaced by a new move, while it content
    // gets saved in killer2 slot.
    if (move != killer1[ply]) {
        killer2[ply] = killer1[ply];
        killer1[ply] = move;
    }

    HistKey k = MakeHistKey(pos, move, refuted);
    int bonus = Inc(depth);

    ApplyHistoryDelta(cutoffHistory[k.piece][k.from][k.to], +bonus);
    ApplyHistoryDelta(refutation[k.side][k.refIndex][k.piece][k.fromBucket][k.to], +bonus);


    return true;
}

// Negative update for the moves that did not cause a beta cutoff
void HistoryData::UpdateTries(Position* pos, const Move move, const Move refuted, const int depth) {

    // Update only for quiet moves
    if (IsMoveNoisy(pos, move))
        return;

    HistKey k = MakeHistKey(pos, move, refuted);
    int bonus = Dec(depth);

    ApplyHistoryDelta(cutoffHistory[k.piece][k.from][k.to], -bonus);
    ApplyHistoryDelta(refutation[k.side][k.refIndex][k.piece][k.fromBucket][k.to], -bonus);
}

// Is a move the killer move for the current ply?
bool HistoryData::IsKiller(const Move move, const int ply) {
    return (move == killer1[ply] || move == killer2[ply]);
}

// Get first killer move
Move HistoryData::GetKiller1(const int ply) {
    return killer1[ply];
}

// Get second killer move
Move HistoryData::GetKiller2(const int ply) {
    return killer2[ply];
}

// Get score (cutoff history + refutation history)
int HistoryData::GetScore(Position* pos, const Move move, const Move refuted) {

    HistKey k = MakeHistKey(pos, move, refuted);

    return cutoffHistory[k.piece][k.from][k.to]
         + refutation[k.side][k.refIndex][k.piece][k.fromBucket][k.to];
}

// Bonus for a beta cutoff
int HistoryData::Inc(const int depth) {
    return std::clamp(128 * depth - 96, 0, 2000);
}

// Penalty for all the moves that failed to produce a beta cutoff
int HistoryData::Dec(const int depth) {
    return std::clamp(112 * depth - 88, 0, 2000);
}

// Update  value in a history array; the nice thing about this  function
// is that entry can be an array of any dimensions, because we are using
// a pointer.
void HistoryData::ApplyHistoryDelta(int16_t& entry, int delta) {

    // delta may be positive (reward) or negative (penalty)
    int bonus = (delta >= 0) ? delta : -delta;

    int old = (int)entry;    // save for diminising returns calculation
    int value = old + delta; // basic updata

    // Diminishing returns
    value -= int((int64_t)old * bonus / maxHist);

    // Clamp and store back
    value = std::clamp(value, -maxHist, maxHist);
    entry = (int16_t)value;
}