// Publius 1.0. Didactic bitboard chess engine by Pawel Koziol

#include "publius.h"
#include "bitboard.h"
#include "bitgen.h"

void MoveGenerator::Init() {

   for (Square square = A1; square < sqNone; ++square) {
       Bitboard b = Paint(square);
       InitPawnAttacks(square, b);
       InitKnightAttacks(square, b);
       InitKingAttacks(square, b);
   }
}

void MoveGenerator::InitPawnAttacks(Square square, Bitboard b) {

    pawnAttacks[White][square] = GetWPAttacks(b);
    pawnAttacks[Black][square] = GetBPAttacks(b);
}

void MoveGenerator::InitKnightAttacks(Square square, Bitboard b) {

    Bitboard west = WestOf(b);
    Bitboard east = EastOf(b);
    knightAttacks[square] = (east | west) << 16;
    knightAttacks[square] |= (east | west) >> 16;
    west = WestOf(west);
    east = EastOf(east);
    knightAttacks[square] |= NorthOf(east | west);
    knightAttacks[square] |= SouthOf(east | west);
}

void MoveGenerator::InitKingAttacks(Square square, Bitboard b) {

    kingAttacks[square] = b;
    kingAttacks[square] |= SidesOf(kingAttacks[square]);
    kingAttacks[square] |= (NorthOf(kingAttacks[square]) | SouthOf(kingAttacks[square]));
    kingAttacks[square] ^= Paint(square);
}

Bitboard MoveGenerator::Pawn(Color c, Square s) { 
	return pawnAttacks[c][s]; 
};

Bitboard MoveGenerator::Knight(Square s) { 
	return knightAttacks[s]; 
};

Bitboard MoveGenerator::Bish(Bitboard o, Square s) {

    Bitboard b = Paint(s);
    return NEOf(FillOcclNE(b, ~o))
         | NWOf(FillOcclNW(b, ~o))
         | SEOf(FillOcclSE(b, ~o))
         | SWOf(FillOcclSW(b, ~o));
}

Bitboard MoveGenerator::Rook(Bitboard o, Square s) {

    Bitboard b = Paint(s);
    return NorthOf(FillOcclNorth(b, ~o))
         | SouthOf(FillOcclSouth(b, ~o))
         | EastOf(FillOcclEast(b, ~o))
         | WestOf(FillOcclWest(b, ~o));
}

Bitboard MoveGenerator::King(Square s) { 
	return kingAttacks[s]; 
};