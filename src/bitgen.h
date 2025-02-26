#pragma once

// bitgen

class MoveGenerator {
private:
	Bitboard pawnAttacks[2][64];
	Bitboard knightAttacks[64];
	Bitboard kingAttacks[64];
	void InitPawnAttacks(const Square sq, const Bitboard b);
	void InitKnightAttacks(const Square sq, const Bitboard b);
	void InitKingAttacks(const Square sq, const Bitboard b);
public:
	void Init(void);
	Bitboard Pawn(const Color color, const Square sq);
	Bitboard Knight(const Square sq);
	Bitboard Bish(const Bitboard occ, const Square sq);
	Bitboard Rook(const Bitboard occ, const Square sq);
	Bitboard Queen(const Bitboard occ, const Square sq);
	Bitboard King(const Square sq);
};

extern MoveGenerator GenerateMoves;