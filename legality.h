// Publius - Didactic public domain bitboard chess engine 
// by Pawel Koziol

#pragma once

bool IsPseudoLegal(Position* pos, int move);
bool IsCastlingLegal(Position* pos, const MoveDescription* md);
bool IsPawnJumpLegal(Position* pos, const MoveDescription* md);
bool IsPawnMoveLegal(const MoveDescription* md);