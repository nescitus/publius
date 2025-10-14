// Publius - Didactic public domain bitboard chess engine 
// by Pawel Koziol

#pragma once

bool IsPseudoLegal(Position* pos, int move);
bool IsCastlingLegal(Position* pos, const Color side, const Square fromSquare, const Square toSquare);
bool IsPawnJumpLegal(Position* pos, const Color side, const PieceType hunter, const PieceType prey,
    const Square fromSquare, const Square toSquare);
bool IsPawnMoveLegal(const Color side, const Square fromSquare, const Square toSquare,
    const Move move, const PieceType prey);