#include "types.h"
#include "square.h"
#include "limits.h"
#include "publius.h"
#include "trans.h"

// Transposition table remembers results
// of the previous searches. If the engine
// encounters the possition it has already
// searched, it can reuse some of the infomation.
// The best case is returning the score, but
// even remembering the best move improves
// move ordering.

void TransTable::Allocate(int mbsize) {

    // Cap the size to a maximum of 1024 MB
	mbsize = std::min(mbsize, 1024);

    // Find the largest power of two less than or equal to mbsize
	for (tableSize = 2; tableSize <= mbsize; tableSize *= 2)
		;

    // Calculate the number of hash records that can fit 
    // in the allocated memory
	tableSize = ((tableSize/2) * 1024 * 1024) / sizeof(hashRecord);

    // Free any previously allocated memory
	free(table);

    // Allocate memory for the transposition table
	table = (hashRecord*) malloc(tableSize * sizeof(hashRecord));

    // Init empty transposition table
	Clear();
}

void TransTable::Exit(void) {
    free(table);
}

void TransTable::Clear(void) {
    std::fill(table, table + tableSize, hashRecord{});
}

bool TransTable::Retrieve(Bitboard key, Move* move, int* score, int* flag, int alpha, int beta, int depth, int ply) {

	hashRecord *slot;

    // Find slot
    slot = FindSlot(key);

    // Make sure hash entry describes current board position
    if (slot->key == key) {

        // We don't know yet if score can be reused,
        // but move can come handy for sorting purposes
		*move = slot->move;
		*flag = slot->flags;

		if (slot->depth >= depth) {

            // Return score, adjusting it for checkmate
			*score = ScoreFromTT(slot->score, ply);

            // Score from the transposition table can be used in search
			if ((slot->flags & lowerBound && *score <= alpha) ||
			    (slot->flags & upperBound && *score >= beta))
				return true;
		}
	}
	return false;
}

void TransTable::Store(Bitboard key, Move move, int score, int flags, int depth, int ply) {

	hashRecord *slot;

    // Adjust checkmate score for root distance
    score = ScoreToTT(score, ply);

    // Find a slot
    slot = FindSlot(key);

    // Don't overwrite better entries
    if (slot->key == key && slot->depth > depth) {
        return;
    }

    // Save the data
    slot->key = key;
    slot->move = move;
    slot->score = score;
    slot->flags = flags;
    slot->depth = depth;
}

// Calculate the slot index using a bitwise AND operation.
// Note that it relies on tableSize being a power of 2.
hashRecord* TransTable::FindSlot(Bitboard key) {
    return table + (key & (tableSize - 4));
}

// ADJUST CHECKMATE SCORE. We must be careful 
// about checkmate scoring. "Mate in n" returned 
// from the transposition table means "mate in n 
// if we start counting n right now". Yet search 
// always returns mate scores as distance 
// from the root, so we must convert to that metric.

int TransTable::ScoreFromTT(int score, int ply) {

    if (score < -EvalLimit)
        score += ply;
    else if (score > EvalLimit)
        score -= ply;

    return score;
}

// Adjust saved checkmate score for root distance
int TransTable::ScoreToTT(int score, int ply) {
    
    if (score < -EvalLimit) 
        score -= ply;
    else if (score > EvalLimit)
        score += ply;

    return score;
}