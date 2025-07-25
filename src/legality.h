// Publius - Didactic public domain bitboard chess engine by Pawel Koziol

#pragma once
bool IsPseudoLegal(Position* pos, int move);
bool IsWhiteShortCastleLegal(Position* pos);
bool IsWhiteLongCastleLegal(Position* pos);
bool IsBlackShortCastleLegal(Position* pos);
bool IsBlackLongCastleLegal(Position* pos);
bool IsPawnJumpLegal(Position* pos, Color side, int hunter, int prey,
    Square fromSquare, Square toSquare);
bool IsPawnMoveLegal(Color side, Square fromSquare, Square toSquare,
    Move move, int hunter, int prey);