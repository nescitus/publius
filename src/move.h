#pragma once

// move

int GetTypeOfMove(const int move);
int GetPromotedPiece(const int move);
Square GetFromSquare(const int move);
Square GetToSquare(const int move);
int CreateMove(const Square fromSquare, const Square toSquare, const int flag);
bool IsMovePromotion(const int move);
bool IsMoveNoisy(Position* pos, const int move);
std::string MoveToString(const int move);
int StringToMove(Position* pos, const std::string& moveString);