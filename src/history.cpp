// Publius - Didactic public domain bitboard chess engine 
// by Pawel Koziol

#include "types.h"
#include "limits.h"
#include "position.h"
#include "move.h"
#include "history.h"

// Constructor
HistoryData::HistoryData() {
    Clear();
}

// Clear all values
void HistoryData::Clear(void) {

    for (ColoredPiece piece = WhitePawn; piece < noPiece; ++piece)
        for (Square square = A1; square < sqNone; ++square) {
            cutoffHistory[piece][square] = 0;
            triesHistory[piece][square] = 0;
        }

    for (int ply = 0; ply < SearchTreeSize; ply++)
        killer1[ply] = killer2[ply] = 0;
}

// Halve history values (used when they grow too high)
void HistoryData::Trim(void) {

    for (ColoredPiece piece = WhitePawn; piece < noPiece; ++piece)
        for (Square square = A1; square < sqNone; ++square) {
            cutoffHistory[piece][square] /= 2;
            triesHistory[piece][square] /= 2;
        }
}

// Update history values on a positive outcome, like a beta cutoff
void HistoryData::Update(Position* pos, const Move move, const int depth, const int ply) {

    // History is updated only for quiet moves
    if (IsMoveNoisy(pos, move))
        return;

    // Update killer moves, taking care
    // that they do not repeat
    if (move != killer2[ply]) {
        killer2[ply] = killer1[ply];
        killer1[ply] = move;
    }

    // Gather data for updating history score
    Square fromSquare = GetFromSquare(move);
    Square toSquare = GetToSquare(move);
    ColoredPiece piece = pos->GetPiece(fromSquare);

    // Update history score
    cutoffHistory[piece][toSquare] += Inc(depth);
 
    // Keep history scores within range
    if (cutoffHistory[piece][toSquare] > HistLimit)
        Trim();
}

void HistoryData::UpdateTries(Position* pos, const Move move, const int depth) {

    // History is updated only for quiet moves
    if (IsMoveNoisy(pos, move)) 
        return;

    // Init
    Square fromSquare = GetFromSquare(move);
    Square toSquare = GetToSquare(move);
    ColoredPiece piece = pos->GetPiece(fromSquare);

    // Update tries history
    triesHistory[piece][toSquare] += Inc(depth);

    // Keep history scores within range
    if (triesHistory[piece][toSquare] > HistLimit)
        Trim();
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

    // How many times the move was considered
    // as an alternative to one that actually
    // produced a beta cutoff?
    int triesCount = triesHistory[piece][toSquare];

    // Avoiding division by zero and giving a new move a decent score
    if (triesCount == 0)
        return 5000;

    // How many times did a move actually cause a cutoff?
    int cutoffsCount = cutoffHistory[piece][toSquare];

    // Return history score in 0..10000 range
    return (10000 * cutoffsCount) / triesCount;
}

int HistoryData::Inc(const int depth) {
    return depth * depth;
}