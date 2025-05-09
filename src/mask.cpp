#include "color.h"
#include "square.h"
#include "limits.h"
#include "publius.h"
#include "bitboard.h"
#include "mask.h"

void MaskData::Init() {

    InitRanks();
    InitPassedMask();
    InitAdjacentMask();
    InitStrongPawn();
}

void MaskData::InitRanks() {

    rank[rank1] = 0x00000000000000FFULL;
    rank[rank2] = 0x000000000000FF00ULL;
    rank[rank3] = 0x0000000000FF0000ULL;
    rank[rank4] = 0x00000000FF000000ULL;
    rank[rank5] = 0x000000FF00000000ULL;
    rank[rank6] = 0x0000FF0000000000ULL;
    rank[rank7] = 0x00FF000000000000ULL;
    rank[rank8] = 0xFF00000000000000ULL;

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

    for (Square s = A1; s < sqNone; ++s) {
        passed[White][s] = FillNorth(NorthOf(Paint(s)));
        passed[White][s] |= SidesOf(passed[White][s]);
        passed[Black][s] = FillSouth(SouthOf(Paint(s)));
        passed[Black][s] |= SidesOf(passed[Black][s]);
    }
}

void MaskData::InitAdjacentMask() {

    for (int f = 0; f < 8; f++)
        adjacentFiles[f] = WestOf(file[f]) | EastOf(file[f]);
}

void MaskData::InitStrongPawn() {

    for (Square square = A1; square < 64; ++square) {
        strongPawn[White][square] = SidesOf(Paint(square));
        strongPawn[Black][square] = SidesOf(Paint(square));
        strongPawn[White][square] |= ForwardOf(strongPawn[White][square], Black);
        strongPawn[Black][square] |= ForwardOf(strongPawn[Black][square], White);
    }
}