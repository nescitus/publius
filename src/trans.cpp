#include "publius.h"
#include "trans.h"

void TransTable::Allocate(int mbsize) {

	mbsize = std::min(mbsize, 1024);

	for (tableSize = 2; tableSize <= mbsize; tableSize *= 2)
		;

	tableSize = ((tableSize/2) * 1024 * 1024) / sizeof(hashRecord);
	free(table);
	table = (hashRecord *)malloc(tableSize * sizeof(hashRecord));
	Clear();
}

void TransTable::Exit(void) {
    free(table);
}

void TransTable::Clear(void) {

    hashRecord *slot;

    for (slot = table; slot < table + tableSize; slot++) {
        slot->key = 0;
        slot->move = 0;
        slot->score = 0;
        slot->flags = 0;
        slot->depth = 0;
    }
}

bool TransTable::Retrieve(Bitboard key, int *move, int *score, int *flag, int alpha, int beta, int depth, int ply) {

	hashRecord *slot;

	slot = table + (key & (tableSize-4));

    // if hash entry is related to current board position,
    // try retrieving information

    if (slot->key == key) {

        // we don't know yet if score can be reused,
        // but move can come handy for sorting purposes

		*move = slot->move;
		*flag = slot->flags;

		if (slot->depth >= depth) {
			*score = slot->score;

            // adjust checkmate score to root distnce

            if (*score < -EvalLimit) {
                *score += ply;
            } else {
                if (*score > EvalLimit) {
                    *score -= ply;
                }
            }

            // score from transposition table can be used in search

			if ((slot->flags & lowerBound && *score <= alpha)
			||  (slot->flags & upperBound && *score >= beta))
				return true;
		}
	}
	return false;
}

void TransTable::Store(Bitboard key, int move, int score, int flags, int depth, int ply) {

	hashRecord *slot;

    // adjust checkmate score for root distance

    if (score < -EvalLimit) {
        score -= ply;
    } else {
        if (score > EvalLimit) {
            score += ply;
        }
    }

    // save data in transposition table

	slot = table + (key & (tableSize - 4));
    slot->key = key;
    slot->move = move;
    slot->score = score;
    slot->flags = flags;
    slot->depth = depth;
}