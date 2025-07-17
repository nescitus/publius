// Publius - Didactic public domain bitboard chess engine by Pawel Koziol

#pragma once

int GetTypeOfMove(const Move move);
int GetPromotedPiece(const Move move);
Square GetFromSquare(const Move move);
Square GetToSquare(const Move move);
Move CreateMove(const Square fromSquare, const Square toSquare, const int flag);
bool IsMovePromotion(const Move move);
bool IsMoveNoisy(Position* pos, const Move move);
std::string MoveToString(const Move move);
Move StringToMove(Position* pos, const std::string& moveString);