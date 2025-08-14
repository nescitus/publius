// Publius - Didactic public domain bitboard chess engine by Pawel Koziol

#pragma once

Color ColorOfPiece(int piece);                     // given a piece value, return its color
int TypeOfPiece(int piece);                        // given a piece value, return its type
int CreatePiece(Color pieceColor, int pieceType);  // get unique id of a piece (denoting, say, white knight), range 0-11