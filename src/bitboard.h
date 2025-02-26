#pragma once

static const Bitboard excludeA = 0xfefefefefefefefe;
static const Bitboard excludeH = 0x7f7f7f7f7f7f7f7f;

Bitboard Paint(const Square s);
Bitboard Paint(const Square s1, const Square s2);
Bitboard Paint(const Square s1, const Square s2, const Square s3);

Square FirstOne(Bitboard b);
int PopCnt(Bitboard b);
Square PopFirstBit(Bitboard* b);

Bitboard NorthOf(const Bitboard b);
Bitboard SouthOf(const Bitboard b);
Bitboard WestOf(const Bitboard b);
Bitboard EastOf(const Bitboard b);
Bitboard NWOf(const Bitboard b);
Bitboard NEOf(const Bitboard b);
Bitboard SEOf(const Bitboard b);
Bitboard SWOf(const Bitboard b);

Bitboard SidesOf(const Bitboard b);
Bitboard ForwardOf(const Bitboard b, const Color c);
Bitboard FrontSpan(const Bitboard b, const Color c);

Bitboard FillNorth(Bitboard b);
Bitboard FillSouth(Bitboard b);
Bitboard GetWPAttacks(const Bitboard b);
Bitboard GetBPAttacks(const Bitboard b);

Bitboard FillOcclSouth(Bitboard b, Bitboard o);
Bitboard FillOcclNorth(Bitboard b, Bitboard o);
Bitboard FillOcclEast(Bitboard b, Bitboard o);
Bitboard FillOcclNE(Bitboard b, Bitboard o);
Bitboard FillOcclSE(Bitboard b, Bitboard o);
Bitboard FillOcclWest(Bitboard b, Bitboard o);
Bitboard FillOcclNW(Bitboard b, Bitboard o);
Bitboard FillOcclSW(Bitboard b, Bitboard o);
