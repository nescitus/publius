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

void HistoryData::ApplyHistoryDelta(int& entry, int delta) {

    // delta may be positive (reward) or negative (penalty)
    int bonus = (delta >= 0) ? delta : -delta;
    int old = entry;

    entry += delta;
    entry -= int((int64_t)old * bonus / maxHist);

    entry = std::clamp(entry, -maxHist, maxHist);
}

// Constructor
HistoryData::HistoryData() {
    Clear();
}

// Clear all values
void HistoryData::Clear(void) {

    for (ColoredPiece piece = WhitePawn; piece < noPiece; ++piece)
        for (Square square1 = A1; square1 < sqNone; ++square1)
            for (Square square2 = A1; square2 < sqNone; ++square2)
                cutoffHistory[piece][square1][square2] = 0;

    for (int ply = 0; ply < SearchTreeSize; ply++)
        killer1[ply] = killer2[ply] = 0;
}

// Update history values on a positive outcome, like a beta cutoff
// Returns true if we actually updated quiet history (so caller may decide
// whether to penalize other quiet tries).
bool HistoryData::Update(Position* pos, const Move move, const int depth, const int ply) {

    // History is updated only for quiet moves
    if (IsMoveNoisy(pos, move))
        return false;

    // Update killer moves, taking care that they do not repeat.
    // Promote killer2 to killer1 if it hits again.
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

    return true;
}

void HistoryData::UpdateTries(Position* pos, const Move move, const int depth) {

    // Update only for quiet moves
    if (IsMoveNoisy(pos, move))
        return;

    // Init
    Square fromSquare = GetFromSquare(move);
    Square toSquare = GetToSquare(move);
    ColoredPiece piece = pos->GetPiece(fromSquare);

    int bonus = Inc(depth);

    ApplyHistoryDelta(cutoffHistory[piece][fromSquare][toSquare], -bonus);
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

int HistoryData::GetScore(Position* pos, const Move move) {

    // Init square variables
    Square fromSquare = GetFromSquare(move);
    Square toSquare = GetToSquare(move);
    ColoredPiece piece = pos->GetPiece(fromSquare);

    return cutoffHistory[piece][fromSquare][toSquare];
}

int HistoryData::Inc(const int depth) {
    return std::clamp(250 * depth - 175, 0, 2000); // +3 Elo
}