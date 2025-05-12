// Publius 1.0. Didactic bitboard chess engine by Pawel Koziol

#include "types.h"
#include "square.h"
#include "limits.h"
#include "publius.h"
#include "bitboard.h"
#include "bitgen.h"

/*
Pawn, knight and king moves are generated
by a table lookup.

Sliding pieces' moves are generated using 
kindergarten bitboards algorithm. It is
a kind of perfect hashing scheme, where
board occupancy is masked (so that we care
only for the relevant rank/file/diagonal)
and boiled down to the interesting bits only
(i.e. to bits that influence the move range).
*/

const Bitboard bbRank1 = 0x00000000000000FF;
const Bitboard bbFileA = 0x0101010101010101;
const Bitboard bbFileB = 0x0202020202020202;
const Bitboard bbA1H8diag = 0x8040201008040201;
const Bitboard bbA8H1diag = 0x0102040810204080;
const Bitboard bbB8H2diag = 0x0204081020408000;

void MoveGenerator::Init() {

    for (Square square = A1; square < sqNone; ++square) {
        Bitboard b = Paint(square);
        InitPawnAttacks(square, b);
        InitKnightAttacks(square, b);
        InitKingAttacks(square, b);
        InitLineMasks(square); // kindergarten bitboards
    }

    InitRankAttacks(); // kindergarten bitboards
    InitFileAttacks(); // kindergarten bitboards
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

// rankAttacks[][] will contain bitboards 
// with exactly the same composition of bits 
// for each rank. If you print such a bitboard,
// you will see entire files filled. In other
// words, you can intersect it with the occupancy
// of any file, and the mask will work the same
// way. Furthermore, if you intersect it with
// a diagonal and ignore bits tthat are unused
// because the diagonal may be too short, 
// the result will also be identical.

void MoveGenerator::InitRankAttacks() {

    int lineIndex, occupancyIndex;

    for (lineIndex = 0; lineIndex < 8; lineIndex++)
        for (occupancyIndex = 0; occupancyIndex < 64; occupancyIndex++) {
            
            // clear data
            rankAttacks[lineIndex][occupancyIndex] = 0;

            // include data for bigger rank indices
            for (int biggerRank = lineIndex + 1; biggerRank < 8; biggerRank++) {
                rankAttacks[lineIndex][occupancyIndex] |= bbFileA << biggerRank;
                if ((1 << biggerRank) & (occupancyIndex << 1)) break;
            }

            // include data for smaller rank indices
            for (int smallerRank = lineIndex - 1; smallerRank >= 0; smallerRank--) {
                rankAttacks[lineIndex][occupancyIndex] |= bbFileA << smallerRank;
                if ((1 << smallerRank) & (occupancyIndex << 1)) break;
            }
        }
}

// Files require different magic formula,
// but are just like ranks, only rotated
// by 90 degrees.

void MoveGenerator::InitFileAttacks() {

    int lineIndex, occupancyIndex;

    for (lineIndex = 0; lineIndex < 8; lineIndex++)
        for (occupancyIndex = 0; occupancyIndex < 64; occupancyIndex++) {
            
            // clear data
            fileAttacks[lineIndex][occupancyIndex] = 0;

            // include data for bigger file indices
            for (int biggerFile = lineIndex + 1; biggerFile < 8; biggerFile++) {
                fileAttacks[lineIndex][occupancyIndex] |= bbRank1 << (biggerFile << 3);
                if ((1 << biggerFile) & (occupancyIndex << 1)) break;
            }

            // include data for smaller file indices
            for (int smallerFile = lineIndex - 1; smallerFile >= 0; smallerFile--) {
                fileAttacks[lineIndex][occupancyIndex] |= bbRank1 << (smallerFile << 3);
                if ((1 << smallerFile) & (occupancyIndex << 1)) break;
            }
        }
}

// Generate masks for full ranks, files, diagonals
// and anti-diagonals. They will be used to extract
// board occupancy data relevant to the moving piece.
void MoveGenerator::InitLineMasks(Square sq) {

    rankMask[sq] = FillOcclEast(Paint(sq), ~0)
                 | FillOcclWest(Paint(sq), ~0);

    fileMask[sq] = FillOcclNorth(Paint(sq), ~0)
                 | FillOcclSouth(Paint(sq), ~0);

    diagonalMask[sq] = FillOcclNE(Paint(sq), ~0)
                     | FillOcclSW(Paint(sq), ~0);

    antiDiagMask[sq] = FillOcclNW(Paint(sq), ~0)
                     | FillOcclSE(Paint(sq), ~0);
}

Bitboard MoveGenerator::Pawn(const Color color, const Square sq) { 
	return pawnAttacks[color][sq]; 
};

Bitboard MoveGenerator::Knight(const Square sq) { 
	return knightAttacks[sq]; 
};

Bitboard MoveGenerator::Bish(const Bitboard occ, const Square sq) {
    return DiagAttacks(occ, sq) | AntiDiagAttacks(occ, sq);
}

Bitboard MoveGenerator::Rook(const Bitboard occ, const Square sq) {
    return FileAttacks(occ, sq) | RankAttacks(occ, sq);
}

Bitboard MoveGenerator::Queen(const Bitboard occ, const Square sq) {
    return Bish(occ, sq) | Rook(occ, sq);
}

Bitboard MoveGenerator::King(const Square sq) {
	return kingAttacks[sq]; 
};

Bitboard MoveGenerator::FileAttacks(const Bitboard occ, const Square sq) {
    
    // occupancy index (fromula for files is different)
    const int occupancyIndex = (((occ) >> ((070 & (sq)) + 1)) & 63);
    
    return (rankAttacks[FileOf(sq)][occupancyIndex] & rankMask[sq]);
}

Bitboard MoveGenerator::RankAttacks(const Bitboard occ, const Square sq) {
    
    // occupancy index
    const int occupancyIndex = (((bbFileA & (occ >> FileOf(sq))) * bbB8H2diag) >> 58);
    
    // fileAttacks[RankOf(sq)][occupancyIndex] contains the same
    // bit pattern for all the files, so that it returns moves
    // for all the squares on the same rank, given the identical
    // occupancy mask. Therefore we need to 'and' it with the mask 
    // of a file that we are interested in.
    return (fileAttacks[RankOf(sq)][occupancyIndex] & fileMask[sq]);
}

Bitboard MoveGenerator::DiagAttacks(const Bitboard occ, const Square sq) {
    
    // masked occupancy
    const Bitboard maskedOccupancy = occ & diagonalMask[sq];
    
    // occupancy index
    const int occupancyIndex = (maskedOccupancy * bbFileB) >> 58;

    // The interesting thing about kindergarten bitboards
    // is that rankAttacks[][] mask that contains information
    // about all the files simultaneously, can be reused for
    // generating diagonal moves. Because most of the diagonals
    // are shorter than 8 squares, some bits are just ignored.
    return (rankAttacks[FileOf(sq)][occupancyIndex] & diagonalMask[sq]);
}

Bitboard MoveGenerator::AntiDiagAttacks(const Bitboard occ, const Square sq) {

    const Bitboard maskedOccupancy = occ & antiDiagMask[sq];
    const int occupancyIndex = (maskedOccupancy * bbFileB) >> 58;
    return (rankAttacks[FileOf(sq)][occupancyIndex] & antiDiagMask[sq]);
}

// Legacy function, showing slower, but more readable algorithm
Bitboard MoveGenerator::BishSlow(const Bitboard occ, const Square sq) {

    Bitboard b = Paint(sq);
    return NEOf(FillOcclNE(b, ~occ))
        | NWOf(FillOcclNW(b, ~occ))
        | SEOf(FillOcclSE(b, ~occ))
        | SWOf(FillOcclSW(b, ~occ));
}

// Legacy function, showing slower, but more readable algorithm
Bitboard MoveGenerator::RookSlow(const Bitboard occ, const Square sq) {

    Bitboard b = Paint(sq);
    return NorthOf(FillOcclNorth(b, ~occ))
        | SouthOf(FillOcclSouth(b, ~occ))
        | EastOf(FillOcclEast(b, ~occ))
        | WestOf(FillOcclWest(b, ~occ));
}