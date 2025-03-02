// Publius 1.0. Didactic bitboard chess engine by Pawel Koziol

#include "color.h"
#include "square.h"
#include "limits.h"
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

void MoveGenerator::InitPawnAttacks(const Square sq, const Bitboard b) {

    pawnAttacks[White][sq] = GetWPAttacks(b);
    pawnAttacks[Black][sq] = GetBPAttacks(b);
}

void MoveGenerator::InitKnightAttacks(const Square sq, const Bitboard b) {

    Bitboard west = WestOf(b);
    Bitboard east = EastOf(b);
    knightAttacks[sq] = (east | west) << 16;
    knightAttacks[sq] |= (east | west) >> 16;
    west = WestOf(west);
    east = EastOf(east);
    knightAttacks[sq] |= NorthOf(east | west);
    knightAttacks[sq] |= SouthOf(east | west);
}

void MoveGenerator::InitKingAttacks(const Square sq, const Bitboard b) {

    kingAttacks[sq] = b;
    kingAttacks[sq] |= SidesOf(kingAttacks[sq]);
    kingAttacks[sq] |= (NorthOf(kingAttacks[sq]) | SouthOf(kingAttacks[sq]));
    kingAttacks[sq] ^= Paint(sq);
}

Bitboard MoveGenerator::Pawn(const Color color, const Square sq) { 
	return pawnAttacks[color][sq]; 
};

Bitboard MoveGenerator::Knight(const Square sq) { 
	return knightAttacks[sq]; 
};

Bitboard MoveGenerator::Bish(const Bitboard occ, const Square sq) {

    Bitboard b = Paint(sq);
    return NEOf(FillOcclNE(b, ~occ))
         | NWOf(FillOcclNW(b, ~occ))
         | SEOf(FillOcclSE(b, ~occ))
         | SWOf(FillOcclSW(b, ~occ));
}

Bitboard MoveGenerator::Rook(const Bitboard occ, const Square sq) {

    Bitboard b = Paint(sq);
    return NorthOf(FillOcclNorth(b, ~occ))
         | SouthOf(FillOcclSouth(b, ~occ))
         | EastOf(FillOcclEast(b, ~occ))
         | WestOf(FillOcclWest(b, ~occ));
}

Bitboard MoveGenerator::Queen(const Bitboard occ, const Square sq) {
    return Bish(occ, sq) | Rook(occ, sq);
}

Bitboard MoveGenerator::King(const Square sq) { 
	return kingAttacks[sq]; 
};