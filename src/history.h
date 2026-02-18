// Publius - Didactic public domain bitboard chess engine
// by Pawel Koziol

#pragma once

#include "types.h"
#include "position.h"
#include "move.h"

class HistoryData {
public:
    HistoryData();

    // Clears history tables and killers
    void ClearOnNewSearch(void);
    void ClearOnNewGame(void);

    // Update history on a quiet beta cutoff.
    // Returns true if quiet history was updated (i.e. move was quiet).
    bool Update(Position* pos, const Move move, const Move refuted, const int depth, const int ply);

    // Penalize a quiet move that was tried before the cutoff move.
    void UpdateTries(Position* pos, const Move move, const Move refuted, const int depth);

    bool IsKiller(const Move move, const int ply);
    Move GetKiller1(const int ply);
    Move GetKiller2(const int ply);

    // Score for quiet move ordering
    int GetScore(Position* pos, const Move move, const Move refuted);

private:

    int Inc(const int depth);
    int Dec(const int depth);
    void ApplyHistoryDelta(int16_t& entry, int delta);

    // History data
    int16_t cutoffHistory[noPiece][sqNone][sqNone];
    int16_t refutation[2][1025][noPiece][sqNone / 4][sqNone];

    // Killer moves per ply
    Move killer1[SearchTreeSize];
    Move killer2[SearchTreeSize];
};

extern HistoryData History;