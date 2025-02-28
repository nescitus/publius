#include "color.h"
#include "square.h"
#include "publius.h"
#include "move.h"
#include "history.h"

void HistoryData::Clear(void) 
{
    for (int piece = 0; piece < 12; piece++)
        for (Square square = A1; square < sqNone; ++square)
            history[piece][square] = 0;

    for (int ply = 0; ply < PlyLimit; ply++) {
        killer1[ply] = 0;
        killer2[ply] = 0;
    }
}

void HistoryData::Trim(void) 
{
    for (int piece = 0; piece < 12; piece++)
        for (Square square = A1; square < sqNone; ++square)
            history[piece][square] /= 2;
}

void HistoryData::Update(Position *pos, int move, int depth, int ply) 
{
    // history is updated only on quiet moves
    if (IsMoveNoisy(pos, move)) {
        return;
    }

    // update killer move
    if (move != killer2[ply]) {
        killer2[ply] = killer1[ply];
        killer1[ply] = move;
    }

    // gather data for updating history score
    Square fromSquare = GetFromSquare(move);
    Square toSquare = GetToSquare(move);
    int piece = pos->GetPiece(fromSquare);

    // update history score
    history[piece][toSquare] += Inc(depth);

    // prevent histoy scores from growing too high
    if (history[piece][toSquare] > HistLimit) {
        Trim();
    }
}

int HistoryData::GetKiller1(const int ply) 
{
    return killer1[ply];
}

int HistoryData::GetKiller2(const int ply)
{
    return killer2[ply];
}

int HistoryData::GetScore(Position *pos, int move) 
{
    return history[pos->GetPiece(GetFromSquare(move))]
                  [GetToSquare(move)];
}

int HistoryData::Inc(const int depth) {
    return depth * depth;
}