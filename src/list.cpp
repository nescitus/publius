// Publius - Didactic public domain bitboard chess engine by Pawel Koziol

#include "types.h"
#include "limits.h"
#include "position.h"
#include "publius.h"
#include "history.h"
#include "move.h"

void MoveList::SwapMoves(const int i, const int j) {

    Move tmpMove = moves[i];
    int tmpVal = values[i];
    moves[i] = moves[j];
    values[i] = values[j];
    moves[j] = tmpMove;
    values[j] = tmpVal;
}

void MoveList::AddMove(Move move) {

    moves[ind] = move;
    values[ind] = 0;
    ind++;
}

void MoveList::AddMove(Square fromSquare, Square toSquare, int flag) {

    moves[ind] = CreateMove(fromSquare, toSquare, flag);
    values[ind] = 0;
    ind++;
}

void MoveList::Clear() { 
    ind = 0; 
    get = 0; 
}

int MoveList::GetInd() {
    return ind;
}

// Get next move from an unsorted list

Move MoveList::GetNextRawMove() {
    if (get < ind)
        return moves[get++];
    return 0; // 0 means "no more moves"
}

// Get the best remaining move from a sorted list

Move MoveList::GetBestMove() { 

    Move move;
    int min = -4 * HistLimit;
    int score = min;
    int loc = 0;

    for (int i = get; i < ind; i++) {
       if (values[i] > score) {
          score = values[i];
          loc = i;
       }
    }

    values[loc] = min;
    move = moves[loc];
    if (loc > get) 
        SwapMoves(loc, get);
    get++;
    return move; 
};

// Score moves that change material balance (captures,
// including en passant, and promotions). Function
// relies on being used within staged move generation
// framework, so that it does not have to compare
// tactical ("noisy") moves with quiet moves.

void MoveList::ScoreNoisy(Position* pos) {

    for (int i = 0; i < ind; i++) {

        // Promotion
        if (IsMovePromotion(moves[i]))
            values[i] = GetPromotedPiece(moves[i]) - 5;

        // Most valuable victim/least valuable attacker
        else if (pos->IsOccupied(GetToSquare(moves[i])))
            values[i] = 6 * pos->PieceTypeOnSq(GetToSquare(moves[i]))
                      + 5 - pos->PieceTypeOnSq(GetFromSquare(moves[i]));
        
        // Default, handles en passant
        else values [i] = 5;
    }
}

// Score quiet moves. Function relies on being called
// within staged move generation framework, so it
// does not have to compare quiet moves with tactical
// ("noisy") moves.

void MoveList::ScoreQuiet(Position* pos) {

    for (int i = 0; i < ind; i++)
        values[i] = History.GetScore(pos, moves[i]);
}