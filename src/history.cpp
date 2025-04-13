#include "color.h"
#include "square.h"
#include "limits.h"
#include "publius.h"
#include "move.h"
#include "history.h"

// constructor
HistoryData::HistoryData() {
    Clear();
}

// clear all values
void HistoryData::Clear(void) 
{
    for (int piece = 0; piece < 12; piece++)
        for (Square square = A1; square < sqNone; ++square) {
            cutoffHistory[piece][square] = 0;
            triesHistory[piece][square] = 0;
        }

    for (int ply = 0; ply < PlyLimit; ply++) {
        killer1[ply] = 0;
        killer2[ply] = 0;
    }
}

// halve history values (used when they grow too high)
void HistoryData::Trim(void) 
{
    for (int piece = 0; piece < 12; piece++)
        for (Square square = A1; square < sqNone; ++square) {
            cutoffHistory[piece][square] /= 2;
            triesHistory[piece][square] /= 2;
        }
}

void HistoryData::Update(Position* pos, const int move, const int depth, const int ply) {

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
    int piece = pos->GetPiece(fromSquare);

    // Update history score
    cutoffHistory[piece][toSquare] += Inc(depth);
 
    // Keep history scores within range
    if (cutoffHistory[piece][toSquare] > HistLimit) {
        Trim();
    }
}

void HistoryData::UpdateTries(Position* pos, const int move, const int depth)
{
    // History is updated only for quiet moves
    if (IsMoveNoisy(pos, move)) 
        return;

    // Init
    Square fromSquare = GetFromSquare(move);
    Square toSquare = GetToSquare(move);
    int piece = pos->GetPiece(fromSquare);

    // Update tries history
    triesHistory[piece][toSquare] += Inc(depth);
 
    // Keep history scores within range
    if (triesHistory[piece][toSquare] > HistLimit)
        Trim();
}

bool HistoryData::IsKiller(const int move, const int ply) 
{
    return (move == killer1[ply] || move == killer2[ply]);
}

int HistoryData::GetKiller1(const int ply) 
{
    return killer1[ply];
}

int HistoryData::GetKiller2(const int ply)
{
    return killer2[ply];
}

int HistoryData::GetScore(Position* pos, const int move) 
{
    // Init square variables
    Square fromSquare = GetFromSquare(move);
    Square toSquare = GetToSquare(move);

    // How many times the move was considered
    // as an alternative to one that actually
    // produced a beta cutoff?
    int triesCount = triesHistory[pos->GetPiece(fromSquare)][toSquare];

    // Avoiding division by zero
    if (triesCount == 0)
        return 0;

    // How many times did a move actually cause a cutoff?
    int cutoffsCount = cutoffHistory[pos->GetPiece(fromSquare)][toSquare];

    // return history score in 0..10000 range
    return (10000 * cutoffsCount) / triesCount;
}

int HistoryData::Inc(const int depth) {
    return depth * depth;
}