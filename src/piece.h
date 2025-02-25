#pragma once

// piece type

enum PieceType { Pawn, Knight, Bishop, Rook, Queen, King, noPieceType };

Color ColorOfPiece(int piece);                     // given a piece value, return its color
int TypeOfPiece(int piece);                        // given a piece value, return its type
int CreatePiece(Color pieceColor, int pieceType);  // get unique id of a piece (denoting, say, white knight), range 0-11
static const int noPiece = 12;                     // constant just outside this range to denote no piece
