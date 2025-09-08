// Publius - Didactic public domain bitboard chess engine 
// by Pawel Koziol

// MaskData class contains various bitmasks used by the engine.

#include "types.h"
#include "bitboard.h"
#include "mask.h"

MaskData::MaskData() {

    InitRanks();
    InitPassedMask();
    InitAdjacentMask();
    InitPawnSupport();

    // Init king attack table
    for (int i = 0; i < 255; i++)
        kingAttack[i] = int(480 * i * i / (i * i + 4200));
}

void MaskData::InitRanks() {

    // rank
    rank[rank1] = 0x00000000000000FFULL;
    rank[rank2] = 0x000000000000FF00ULL;
    rank[rank3] = 0x0000000000FF0000ULL;
    rank[rank4] = 0x00000000FF000000ULL;
    rank[rank5] = 0x000000FF00000000ULL;
    rank[rank6] = 0x0000FF0000000000ULL;
    rank[rank7] = 0x00FF000000000000ULL;
    rank[rank8] = 0xFF00000000000000ULL;

    // file
    file[fileA] = 0x0101010101010101ULL;
    file[fileB] = 0x0202020202020202ULL;
    file[fileC] = 0x0404040404040404ULL;
    file[fileD] = 0x0808080808080808ULL;
    file[fileE] = 0x1010101010101010ULL;
    file[fileF] = 0x2020202020202020ULL;
    file[fileG] = 0x4040404040404040ULL;
    file[fileH] = 0x8080808080808080ULL;

    // relative rank
    for (int i = 0; i < 8; i++) {
        rr[White][i] = rank[i];
        rr[Black][i] = rank[7 - i];
    }
}

void MaskData::InitPassedMask() {

    for (Square square = A1; square < sqNone; ++square) {
        passed[White][square] = FillNorth(NorthOf(Paint(square)));
        passed[White][square] |= SidesOf(passed[White][square]);
        passed[Black][square] = FillSouth(SouthOf(Paint(square)));
        passed[Black][square] |= SidesOf(passed[Black][square]);
    }
}

void MaskData::InitAdjacentMask() {

    for (int f = 0; f < 8; f++)
        adjacentFiles[f] = WestOf(file[f]) | EastOf(file[f]);
}

void MaskData::InitPawnSupport() {

    for (Square square = A1; square < 64; ++square) {

        Bitboard base = SidesOf(Paint(square));

        // supported pawn has own pawn on a neighbourig rank
        // that is not more advanced than apawn in question
        support[White][square] = base | FillSouth(base);
        support[Black][square] = base | FillNorth(base);

        // strong pawn has a neighbour side by side or is defended
        strongPawn[White][square] = base | SouthOf(base);
        strongPawn[Black][square] = base | NorthOf(base);
    }
}