// Publius - Didactic public domain bitboard chess engine
// by Pawel Koziol

#include <cstdint>
#include <algorithm> // std::clamp
#include "types.h"
#include "limits.h"
#include "position.h"
#include "move.h"
#include "history.h"

const int maxHist = 16384;

// Constructor
HistoryData::HistoryData() {
    Clear();
    ClearRefutation();
}

void HistoryData::ClearRefutation() {
    std::memset(refutation, 0, sizeof(refutation));
}

// Clear all values
void HistoryData::Clear(void) {

    std::memset(cutoffHistory, 0, sizeof(cutoffHistory));
   // std::memset(refutation, 0, sizeof(refutation)); // only on new game

    for (int ply = 0; ply < SearchTreeSize; ply++)
        killer1[ply] = killer2[ply] = 0;
}

// Update history values on a positive outcome, like a beta cutoff.
// Returns true if we actually updated quiet history (so caller may decide
// whether to penalize other quiet tries).
bool HistoryData::Update(Position* pos, const Move move, const Move oldMove, const int depth, const int ply) {

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

    // Gather data for updating history score
    Square fromSquare = GetFromSquare(move);
    Square toSquare = GetToSquare(move);
    ColoredPiece piece = pos->GetPiece(fromSquare);

    int bonus = Inc(depth);

    ApplyHistoryDelta(cutoffHistory[piece][fromSquare][toSquare], +bonus);
    ApplyHistoryDelta(refutation[GetFromSquare(oldMove)][GetToSquare(oldMove)][piece][fromSquare][toSquare], +bonus);

    return true;
}

void HistoryData::UpdateTries(Position* pos, const Move move, const Move oldMove, const int depth) {

    // Update only for quiet moves
    if (IsMoveNoisy(pos, move))
        return;

    // Init
    Square fromSquare = GetFromSquare(move);
    Square toSquare = GetToSquare(move);
    ColoredPiece piece = pos->GetPiece(fromSquare);

    int bonus = Inc(depth);

    ApplyHistoryDelta(cutoffHistory[piece][fromSquare][toSquare], -bonus);
    ApplyHistoryDelta(refutation[GetFromSquare(oldMove)][GetToSquare(oldMove)][piece][fromSquare][toSquare], -bonus);
}

bool HistoryData::IsKiller(const Move move, const int ply) {
    return (move == killer1[ply] || move == killer2[ply]);
}

Move HistoryData::GetKiller1(const int ply) {
    return killer1[ply];
}

Move HistoryData::GetKiller2(const int ply) {
    return killer2[ply];
}

int HistoryData::GetScore(Position* pos, const Move move, const Move oldMove) {

    // Init square variables
    Square fromSquare = GetFromSquare(move);
    Square toSquare = GetToSquare(move);
    ColoredPiece piece = pos->GetPiece(fromSquare);

    return cutoffHistory[piece][fromSquare][toSquare]
         + refutation[GetFromSquare(oldMove)][GetToSquare(oldMove)][piece][fromSquare][toSquare];
}

int HistoryData::Inc(const int depth) {
    return std::clamp(128 * depth - 96, 0, 2000); // +3 Elo
}

// Update  value in a history array; the nice thing about this  function
// is that entry can be an array of any dimensions, because we are using
// a pointer.
void HistoryData::ApplyHistoryDelta(int& entry, int delta) {

    // delta may be positive (reward) or negative (penalty)
    int bonus = (delta >= 0) ? delta : -delta;
    int old = entry;

    // Basic update
    entry += delta;

    // Effectively  undoing part of the change. The  larger  abs(entry)
    // already is, the smaller the effective change, so history doesn't 
    // "run away" and can both learn and unlearn smoothly.
    entry -= int((int64_t)old * bonus / maxHist);

    // Make sure entry value stays within bounds, just in case.
    entry = std::clamp(entry, -maxHist, maxHist);
}