#pragma once

static const Bitboard excludeA = 0xfefefefefefefefe;
static const Bitboard excludeH = 0x7f7f7f7f7f7f7f7f;

Bitboard Paint(Square s);
Bitboard Paint(Square s1, Square s2);
Bitboard Paint(Square s1, Square s2, Square s3);

Square FirstOne(Bitboard b);
int PopCnt(Bitboard b);
Square PopFirstBit(Bitboard* b);

Bitboard NorthOf(Bitboard b);
Bitboard SouthOf(Bitboard b);
Bitboard WestOf(Bitboard b);
Bitboard EastOf(Bitboard b);
Bitboard NWOf(Bitboard b);
Bitboard NEOf(Bitboard b);
Bitboard SEOf(Bitboard b);
Bitboard SWOf(Bitboard b);

Bitboard SidesOf(Bitboard b);
Bitboard ForwardOf(Bitboard b, Color c);
Bitboard FrontSpan(Bitboard b, Color c);

Bitboard FillNorth(Bitboard b);
Bitboard FillSouth(Bitboard b);
Bitboard GetWPAttacks(Bitboard b);
Bitboard GetBPAttacks(Bitboard b);

Bitboard FillOcclSouth(Bitboard b, Bitboard o);
Bitboard FillOcclNorth(Bitboard b, Bitboard o);
Bitboard FillOcclEast(Bitboard b, Bitboard o);
Bitboard FillOcclNE(Bitboard b, Bitboard o);
Bitboard FillOcclSE(Bitboard b, Bitboard o);
Bitboard FillOcclWest(Bitboard b, Bitboard o);
Bitboard FillOcclNW(Bitboard b, Bitboard o);
Bitboard FillOcclSW(Bitboard b, Bitboard o);
