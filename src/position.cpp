#include "types.h"
#include "piece.h"
#include "square.h"
#include "limits.h"
#include "publius.h"
#include "bitboard.h"
#include "bitgen.h"
#include "hashkeys.h"

// For Position class members observe the following 
// naming convention: if the function to manipulate
// board position doesn't change the hashkey, then
// it has "NoHash" appended to the name. Otherwise,
// expect any manipulator function to modify
// the hash key.

void Position::Clear() {

    for (Color color = White; color < colorNone; ++color) {
        
        // Clear king square
        kingSq[color] = sqNone;
    
        // Clear bitboards and piece counts
        for (int type = 0; type < 6; type++) {
            pieceBitboard[color][type] = 0ULL;
            pieceCount[color][type] = 0;
        }
    }

    // Clear piece locations
    for (Square square = A1; square < 64; ++square)
        pieceLocation[square] = noPiece;

    // Clear single variables
    castleFlags = 0;
    reversibleMoves = 0;
    repetitionIndex = 0;
    enPassantSq = sqNone;
    sideToMove = White;
}

void Position::Set(const std::string& str) {

    Clear();
    
    int length = str.length();
    Square square = A1;

    for (int i = 0; i < length; i++) {
        char letter = str.at(i);

        if (square < 64) {
            switch (letter) {
                case '/': break;
                case '1': 
                    square = square + 1; 
                    break;
                case '2': 
                    square = square + 2; 
                    break;
                case '3': 
                    square = square + 3; 
                    break;
                case '4': 
                    square = square + 4; 
                    break;
                case '5': 
                    square = square + 5; 
                    break;
                case '6': 
                    square = square + 6; 
                    break;
                case '7': 
                    square = square + 7; 
                    break;
                case '8': 
                    square = square + 8; 
                    break;
                case 'p': 
                    AddPieceNoHash(Black, Pawn, InvertSquare(square)); 
                    ++square; 
                    break;
                case 'b': 
                    AddPieceNoHash(Black, Bishop, InvertSquare(square)); 
                    ++square; 
                    break;
                case 'n': 
                    AddPieceNoHash(Black, Knight, InvertSquare(square)); 
                    ++square; 
                    break;
                case 'r': 
                    AddPieceNoHash(Black, Rook, InvertSquare(square)); 
                    ++square; 
                    break;
                case 'q': 
                    AddPieceNoHash(Black, Queen, InvertSquare(square)); 
                    ++square; 
                    break;
                case 'k': 
                    AddPieceNoHash(Black, King, InvertSquare(square)); 
                    kingSq[Black] = InvertSquare(square); 
                    ++square; 
                    break;
                case 'P': 
                    AddPieceNoHash(White, Pawn, InvertSquare(square)); 
                    ++square; 
                    break;
                case 'B': 
                    AddPieceNoHash(White, Bishop, InvertSquare(square)); 
                    ++square; 
                    break;
                case 'N': 
                    AddPieceNoHash(White, Knight, InvertSquare(square)); 
                    ++square; 
                    break;
                case 'R': 
                    AddPieceNoHash(White, Rook, InvertSquare(square)); 
                    ++square; 
                    break;
                case 'Q': 
                    AddPieceNoHash(White, Queen, InvertSquare(square)); 
                    ++square; 
                    break;
                case 'K': 
                    AddPieceNoHash(White, King, InvertSquare(square)); 
                    kingSq[White] = InvertSquare(square); 
                    ++square; 
                    break;
            }
        } else { // all pieces are in place, now deal with the other data

            switch (letter) {
                case 'w':
                    sideToMove = White;
                    break;
                case 'b':
                    if (str.at(i+1) == '3') enPassantSq = B3;
                    else if (str.at(i+1) == '6') enPassantSq = B6;
                    else sideToMove = Black;
                    break;
                case 'k':
                    castleFlags |= bShortCastle;
                    break;
                case 'K':
                    castleFlags |= wShortCastle;
                    break;
                case 'q':
                    castleFlags |= bLongCastle;
                    break;
                case 'Q':
                    castleFlags |= wLongCastle;
                    break;
                case 'a':
                    str.at(i+1) == '3' ? enPassantSq = A3 : enPassantSq = A6;
                    break;
                case 'c':
                    str.at(i+1) == '3' ? enPassantSq = C3 : enPassantSq = C6;
                    break;
                case 'd':
                    str.at(i+1) == '3' ? enPassantSq = D3 : enPassantSq = D6;
                    break;
                case 'e':
                    str.at(i+1) == '3' ? enPassantSq = E3 : enPassantSq = E6;
                    break;
                case 'f':
                    str.at(i+1) == '3' ? enPassantSq = F3 : enPassantSq = F6;
                    break;
                case 'g':
                    str.at(i+1) == '3' ? enPassantSq = G3 : enPassantSq = G6;
                    break;
                case 'h':
                    str.at(i+1) == '3' ? enPassantSq = H3 : enPassantSq = H6;
                    break;
            }
        }
    }
    
    boardHash = CalculateHashKey();
    pawnHash = CalculatePawnKey();
}

Bitboard Position::CalculateHashKey() {

    Bitboard key = 0;

    for (Square square = A1; square < 64; ++square)
        if (pieceLocation[square] != noPiece)
            key ^= Key.pieceKey[pieceLocation[square]][square];

    key ^= Key.castleKey[castleFlags];

    if (enPassantSq != sqNone)
        key ^= Key.enPassantKey[FileOf(enPassantSq)];

    if (sideToMove == Black)
        key ^= sideRandom;

    return key;
}

Bitboard Position::CalculatePawnKey() {

    Bitboard key = 0;

    for (Square square = A1; square < 64; ++square)
        if (pieceLocation[square] != noPiece) {
            if (PieceTypeOnSq(square) == Pawn || PieceTypeOnSq(square) == King)
               key ^= Key.pieceKey[pieceLocation[square]][square];
        }

    return key;
}

void Position::SwitchSide() {

     sideToMove = ~sideToMove;
     boardHash ^= sideRandom;
}

void Position::ClearEnPassant() {

     if (enPassantSq != sqNone) {
         boardHash ^= Key.enPassantKey[FileOf(enPassantSq)];
         enPassantSq = sqNone;
     }
}

void Position::MovePiece(const Color color, const int hunter, 
                         const Square fromSquare, const Square toSquare) {

    MovePieceNoHash(color, hunter, fromSquare, toSquare);
    boardHash ^= Key.ForPiece(color, hunter, fromSquare) ^
                 Key.ForPiece(color, hunter, toSquare);
}

void Position::MovePieceNoHash(const Color color, const int hunter, 
                               const Square fromSquare, Square toSquare) {

     pieceLocation[fromSquare] = noPiece;
     pieceLocation[toSquare] = CreatePiece(color, hunter);
     pieceBitboard[color][hunter] ^= Paint(fromSquare, toSquare);
}

void Position::TakePiece(const Color color, 
                         const int typeOfPiece, 
                         const Square square) {

    TakePieceNoHash(color, typeOfPiece, square);
    boardHash ^= Key.ForPiece(color, typeOfPiece, square);
}

void Position::TakePieceNoHash(const Color color, 
                               const int typeOfPiece, 
                               const Square square) {

     pieceLocation[square] = noPiece;
     pieceBitboard[color][typeOfPiece] ^= Paint(square);
     pieceCount[color][typeOfPiece]--;
}

void Position::AddPieceNoHash(const Color color, 
                              const int typeOfPiece, 
                              const Square square) {

     pieceLocation[square] = CreatePiece(color,typeOfPiece);
     pieceBitboard[color][typeOfPiece] ^= Paint(square);
     pieceCount[color][typeOfPiece]++;
}

void Position::ChangePieceNoHash(const int oldType, 
                                 const int newType, 
                                 const Color color, 
                                 const Square square) {

     pieceLocation[square] = CreatePiece(color, newType);
     pieceBitboard[color][oldType] ^= Paint(square);
     pieceBitboard[color][newType] ^= Paint(square);
     pieceCount[color][newType]++;
     pieceCount[color][oldType]--;
}

void Position::SetEnPassantSquare(const Color color, Square toSquare) {

    toSquare = toSquare ^ 8;
    if (GenerateMoves.Pawn(color, toSquare) & (pieceBitboard[~color][Pawn])) {
        enPassantSq = toSquare;
        boardHash ^= Key.enPassantKey[FileOf(toSquare)];
    }
}

void Position::UpdateCastlingRights(const Square fromSquare, const Square toSquare) {

    boardHash ^= Key.castleKey[castleFlags];
    castleFlags &= castleMask[fromSquare] & castleMask[toSquare];
    boardHash ^= Key.castleKey[castleFlags];
}

void Position::TryMarkingIrreversible() {

    if (reversibleMoves == 0)
        repetitionIndex = 0;
}
