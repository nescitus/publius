// Publius - Didactic public domain bitboard chess engine 
// by Pawel Koziol

#pragma once

inline void SerializeMoves(MoveList* list, const Square fromSquare, Bitboard moves);
inline void SerializePawnMoves(MoveList* list, Bitboard moves, int vector, int flag);
void SerializePromotions(MoveList* list, Bitboard moves, int vector);
void FillQuietList(Position* pos, MoveList* list);
void FillNoisyList(Position* pos, MoveList* list);
void FillCheckList(Position* pos, MoveList* list);