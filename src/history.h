// Publius - Didactic public domain bitboard chess engine by Pawel Koziol

#pragma once

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
    void Clear(void);

    // Update history on a quiet beta cutoff.
    // Returns true if quiet history was updated (i.e. move was quiet).
    bool Update(Position* pos, const Move move, const int depth, const int ply);

    // Penalize a quiet move that was tried before the cutoff move.
    void UpdateTries(Position* pos, const Move move, const int depth);

    bool IsKiller(const Move move, const int ply);
    Move GetKiller1(const int ply);
    Move GetKiller2(const int ply);

    // Score for quiet move ordering
    int GetScore(Position* pos, const Move move);

private:

    int Inc(const int depth);
    void ApplyHistoryDelta(int& entry, int delta);

    // Signed history in range [-H, +H]
    int cutoffHistory[noPiece][sqNone][sqNone];

    // Killer moves per ply
    Move killer1[SearchTreeSize];
    Move killer2[SearchTreeSize];
};

extern HistoryData History;