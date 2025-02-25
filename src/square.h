#pragma once

Square MakeSquare(int rank, int file);
int RankOf(Square square);
int FileOf(Square square);
Square InvertSquare(Square square);
Square RelativeSq(Color color, Square square);