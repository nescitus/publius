#pragma once

// bitgen

class MoveGenerator {
private:
    Bitboard pawnAttacks[2][64];
    Bitboard knightAttacks[64];
    Bitboard kingAttacks[64];
    Bitboard rankAttacks[8][64];
    Bitboard fileAttacks[8][64];
    Bitboard rankMask[64];
    Bitboard fileMask[64];
    Bitboard diagonalMask[64];
    Bitboard antiDiagMask[64];
    void InitPawnAttacks(const Square sq, const Bitboard b);
    void InitKnightAttacks(const Square sq, const Bitboard b);
    void InitKingAttacks(const Square sq, const Bitboard b);
    void InitLineMasks(const Square sq);
    Bitboard FileAttacks(const Bitboard occ, const Square sq);
    Bitboard RankAttacks(const Bitboard occ, const Square sq);
    Bitboard DiagAttacks(const Bitboard occ, const Square sq);
    Bitboard AntiDiagAttacks(const Bitboard occ, const Square sq);
    void InitRankAttacks();
    void InitFileAttacks();
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