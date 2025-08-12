// Publius - Didactic public domain bitboard chess engine by Pawel Koziol

#pragma once

static const Bitboard excludeA = 0xfefefefefefefefe;
static const Bitboard excludeH = 0x7f7f7f7f7f7f7f7f;

Bitboard Paint(const Square s);
Bitboard Paint(const Square s1, const Square s2);
Bitboard Paint(const Square s1, const Square s2, const Square s3);

Square FirstOne(Bitboard b);
int PopCnt(Bitboard b);
Square PopFirstBit(Bitboard* b);

// Helpers to shift a bitboard in the given direction.
// They are named after cardinal directions and we assume
// that white moves north, that is up the board,
// like on a chess diagram.

constexpr inline Bitboard NorthOf(Bitboard b) { return(b << 8); }
constexpr inline Bitboard SouthOf(Bitboard b) { return (b >> 8); }
constexpr inline Bitboard WestOf(Bitboard b) { return ((b & excludeA) >> 1); }
constexpr inline Bitboard EastOf(Bitboard b) { return ((b & excludeH) << 1); }
constexpr inline Bitboard NWOf(Bitboard b) { return ((b & excludeA) << 7); }
constexpr inline Bitboard NEOf(Bitboard b) { return ((b & excludeH) << 9); }
constexpr inline Bitboard SWOf(Bitboard b) { return ((b & excludeA) >> 9); }
constexpr inline Bitboard SEOf(Bitboard b) { return ((b & excludeH) >> 7); }

// side-dependent forward shift
constexpr inline Bitboard ForwardOf(Bitboard b, Color color) {
    return (color == White) ? NorthOf(b) : SouthOf(b);
}
// shift the bitboard to both sides
constexpr inline Bitboard SidesOf(Bitboard b) {
    return (WestOf(b) | EastOf(b));
}

// get pawn bitboard attacks

constexpr inline Bitboard GetWPAttacks(const Bitboard b) {
    return (NEOf(b) | NWOf(b));
}

constexpr inline Bitboard GetBPAttacks(const Bitboard b) {
    return (SEOf(b) | SWOf(b));
}

Bitboard FrontSpan(const Bitboard b, const Color color);

Bitboard FillNorth(Bitboard b);
Bitboard FillSouth(Bitboard b);
Bitboard FillOcclSouth(Bitboard b, Bitboard o);
Bitboard FillOcclNorth(Bitboard b, Bitboard o);
Bitboard FillOcclEast(Bitboard b, Bitboard o);
Bitboard FillOcclNE(Bitboard b, Bitboard o);
Bitboard FillOcclSE(Bitboard b, Bitboard o);
Bitboard FillOcclWest(Bitboard b, Bitboard o);
Bitboard FillOcclNW(Bitboard b, Bitboard o);
Bitboard FillOcclSW(Bitboard b, Bitboard o);

Bitboard RelSqBb(const Color cl, const Square sq);
