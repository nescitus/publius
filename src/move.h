#pragma once

// move

int GetTypeOfMove(int move);
int GetPromotedPiece(int move);
Square GetFromSquare(int move);
Square GetToSquare(int move);
int CreateMove(Square fromSquare, Square toSquare, int flag);
bool IsMovePromotion(int move);
bool IsMoveNoisy(Position* pos, int move);
std::string MoveToString(int move);
int StringToMove(Position* pos, const std::string& moveString);