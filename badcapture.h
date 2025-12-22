// Publius - Didactic public domain bitboard chess engine 
// by Pawel Koziol

#pragma once

// piece values for static exchange evaluation
const int pieceValue[6] = { 100, 300, 300, 500, 900, 0 };

bool IsBadCapture(Position* pos, Move move);
int Swap(const Position* pos, const Square fromSquare, const Square toSquare);
