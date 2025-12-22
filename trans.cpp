// Publius - Didactic public domain bitboard chess engine 
// by Pawel Koziol

#include "types.h"
#include "limits.h"
#include "position.h"
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
    tt_date = 0;
}

void TransTable::Age(void) {
    tt_date = (tt_date + 1) & 255;
}

bool TransTable::Retrieve(Bitboard key, Move* move, int* score, int* flag, int alpha, int beta, int depth, int ply) {

    hashRecord *slot;

    // Find the first slot eligible for holding relevant data
    slot = FindFirstSlot(key);

    // Look at a couple of slots where information
    // related to the current position might be saved
    for (int i = 0; i < numberOfBuckets; i++) {
       
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
                if ((slot->flags & upperBound && *score <= alpha) ||
                    (slot->flags & lowerBound && *score >= beta))
                    return true;
            }
            break;
        }
        slot++;
    }
    return false;
}

void TransTable::Store(Bitboard key, Move move, int score, int flags, int depth, int ply) {

    hashRecord *slot;
    hashRecord* replace = NULL;
    int oldest, age;

    // Adjust checkmate score for root distance
    score = ScoreToTT(score, ply);

    // Find the first slot eligible for saving data
    slot = FindFirstSlot(key);
    oldest = -1;

    // Look at a couple of neighbouring slots,
    // deciding which one holds the least valuable
    // information and can be overwritten.
    for (int i = 0; i < numberOfBuckets; i++) {

        // Position already recorded, updating it 
        // has absolute priority over finding a new slot
        if (slot->key == key) {
            if (!move) move = slot->move;
            replace = slot;
            break;
        }
        
        // Update by age or depth...
        age = ((tt_date - slot->date) & 255) * 256 + 255 - slot->depth;
        
        // ... but prefer unused entries
        if (slot->key == 0) age = 1024;

        if (age > oldest) {
            oldest = age;
            replace = slot;
        }
        
        slot++;
    }

    // Save the data
    replace->key = key;
    replace->date = tt_date;
    replace->move = move;
    replace->score = score;
    replace->flags = flags;
    replace->depth = depth;
}

// Calculate the slot index using a bitwise AND operation.
// Note that it relies on tableSize being a power of 2.
hashRecord* TransTable::FindFirstSlot(Bitboard key) {
    return table + (key & (tableSize - numberOfBuckets));
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