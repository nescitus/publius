#pragma once

// bitgen

class MoveGenerator {
private:
	Bitboard pawnAttacks[2][64];
	Bitboard knightAttacks[64];
	Bitboard kingAttacks[64];
	void InitPawnAttacks(Square square, Bitboard b);
	void InitKnightAttacks(Square square, Bitboard b);
	void InitKingAttacks(Square square, Bitboard b);
public:
	void Init(void);
	Bitboard Pawn(Color c, Square s);
	Bitboard Knight(Square s);
	Bitboard Bish(Bitboard o, Square s);
	Bitboard Rook(Bitboard o, Square s);
	Bitboard Queen(Bitboard o, Square s);
	Bitboard King(Square s);
};

extern MoveGenerator GenerateMoves;