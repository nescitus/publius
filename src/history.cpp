#include "color.h"
#include "square.h"
#include "publius.h"
#include "move.h"
#include "history.h"

void cHistory::Clear(void) 
{
    for (int piece = 0; piece < 12; piece++)
        for (Square square = A1; square < sqNone; ++square)
            history[piece][square] = 0;

    for (int ply = 0; ply < PlyLimit; ply++) {
        killer[ply] = 0;
    }
}

void cHistory::Trim(void) 
{
    for (int piece = 0; piece < 12; piece++)
        for (Square square = A1; square < sqNone; ++square)
            history[piece][square] /= 2;
}

void cHistory::Update(Position *pos, int move, int depth, int ply) 
{
    if (IsMoveNoisy(pos, move)) {
        return;
    }

    killer[ply] = move;

    Square fromSquare = GetFromSquare(move);
    Square toSquare = GetToSquare(move);
    int piece = pos->GetPiece(fromSquare);

    history[piece][toSquare] += depth * depth;

    if (history[piece][toSquare] > HistLimit) {
        Trim();
    }
}

int cHistory::GetKiller(int ply) 
{
    return killer[ply];
}

int cHistory::Get(Position *pos, int move) 
{
    return history[pos->GetPiece(GetFromSquare(move))]
                  [GetToSquare(move)];
}