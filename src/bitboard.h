// Publius - Didactic public domain bitboard chess engine 
// by Pawel Koziol

#pragma once

// Functions converting square(s) to a corresponding bitboard

[[nodiscard]] constexpr inline Bitboard Paint(const Square s) {
    return 1ULL << s;
}

[[nodiscard]] constexpr inline Bitboard Paint(const Square s1, const Square s2) {
    return Paint(s1) | Paint(s2);
}

[[nodiscard]] constexpr inline Bitboard Paint(const Square s1, const Square s2, const Square s3) {
    return Paint(s1, s2) | Paint(s3);
}

// Functions moved to popcount.cpp
// in order to handle compiler switch separately

[[nodiscard]] Square FirstOne(Bitboard b);
[[nodiscard]] int PopCnt(Bitboard b);
[[nodiscard]] Square PopFirstBit(Bitboard* b);

// Masks used for shift helpers

static const Bitboard excludeA = 0xfefefefefefefefe;
static const Bitboard excludeH = 0x7f7f7f7f7f7f7f7f;

// Helpers to shift a bitboard in the given direction.
// They are named after cardinal directions and we assume
// that white moves north, that is up the board,
// like on a chess diagram.

[[nodiscard]] constexpr inline Bitboard NorthOf(Bitboard b) { return(b << 8); }
[[nodiscard]] constexpr inline Bitboard SouthOf(Bitboard b) { return (b >> 8); }
[[nodiscard]] constexpr inline Bitboard WestOf(Bitboard b) { return ((b & excludeA) >> 1); }
[[nodiscard]] constexpr inline Bitboard EastOf(Bitboard b) { return ((b & excludeH) << 1); }
[[nodiscard]] constexpr inline Bitboard NWOf(Bitboard b) { return ((b & excludeA) << 7); }
[[nodiscard]] constexpr inline Bitboard NEOf(Bitboard b) { return ((b & excludeH) << 9); }
[[nodiscard]] constexpr inline Bitboard SWOf(Bitboard b) { return ((b & excludeA) >> 9); }
[[nodiscard]] constexpr inline Bitboard SEOf(Bitboard b) { return ((b & excludeH) >> 7); }

// side-dependent forward shift
[[nodiscard]] constexpr inline Bitboard ForwardOf(Bitboard b, Color color) {
    return (color == White) ? NorthOf(b) : SouthOf(b);
}
// shift the bitboard to both sides
[[nodiscard]] constexpr inline Bitboard SidesOf(Bitboard b) {
    return (WestOf(b) | EastOf(b));
}

// bitboard of white pawn attacks
[[nodiscard]] constexpr inline Bitboard GetWPAttacks(const Bitboard b) {
    return (NEOf(b) | NWOf(b));
}

// bitboard of black pawn attacks
[[nodiscard]] constexpr inline Bitboard GetBPAttacks(const Bitboard b) {
    return (SEOf(b) | SWOf(b));
}

// Simple fill functions in the most useful directions

[[nodiscard]] Bitboard FillForward(const Bitboard b, const Color color);

[[nodiscard]] constexpr inline Bitboard FillNorth(Bitboard b) {
    b |= b << 8;
    b |= b << 16;
    b |= b << 32;
    return b;
}

[[nodiscard]] constexpr inline Bitboard FillSouth(Bitboard b) {
    b |= b >> 8;
    b |= b >> 16;
    b |= b >> 32;
    return b;
}

// Occluded fill functions. Currently we use them
// only to initialize kindergarten bitboards, but
// you could base move generator on them. For details,
// see https://www.chessprogramming.org/Kogge-Stone_Algorithm#Occluded_Fill

[[nodiscard]] Bitboard FillOcclSouth(Bitboard b, Bitboard o);
[[nodiscard]] Bitboard FillOcclNorth(Bitboard b, Bitboard o);
[[nodiscard]] Bitboard FillOcclEast(Bitboard b, Bitboard o);
[[nodiscard]] Bitboard FillOcclNE(Bitboard b, Bitboard o);
[[nodiscard]] Bitboard FillOcclSE(Bitboard b, Bitboard o);
[[nodiscard]] Bitboard FillOcclWest(Bitboard b, Bitboard o);
[[nodiscard]] Bitboard FillOcclNW(Bitboard b, Bitboard o);
[[nodiscard]] Bitboard FillOcclSW(Bitboard b, Bitboard o);

// Bitboard of a relative square
// - RelativeSq(White, C2) is C2,
// - RelativeSq(Black, C2) is C7
[[nodiscard]] Bitboard RelativeSqBb(const Color cl, const Square sq);
