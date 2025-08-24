// Publius - Didactic public domain bitboard chess engine by Pawel Koziol

#pragma once
bool IsPseudoLegal(Position* pos, int move);
bool IsCastlingLegal(Position* pos, Color side, Square fromSquare, Square toSquare);
bool IsPawnJumpLegal(Position* pos, Color side, PieceType hunter, PieceType prey,
    Square fromSquare, Square toSquare);
bool IsPawnMoveLegal(Color side, Square fromSquare, Square toSquare,
    Move move, PieceType hunter, PieceType prey);