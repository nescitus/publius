// Publius - Didactic public domain bitboard chess engine by Pawel Koziol

#include <tuple>
#include <cctype> // isdigit
#include "types.h"
#include "piece.h"
#include "square.h"
#include "mask.h"
#include "position.h"
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
        for (int pieceType = 0; pieceType < 6; pieceType++) {
            pieceBitboard[color][pieceType] = 0ULL;
            pieceCount[color][pieceType] = 0;
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

        // The first loop sets up the pieves
        if (square < 64) {

            if (std::isdigit(letter)) {
               square = square + (int)(letter - '0');
            } else {
                auto tuple = PieceFromChar(letter);
                Color color = std::get<0>(tuple);
                int pieceType = std::get<1>(tuple);
                if (color != colorNone) {
                    AddPieceNoHash(color, pieceType, MirrorRank(square));
                    if (pieceType == King)
                        kingSq[color] = MirrorRank(square);
                    ++square;
                }
            }

        // the second loop sets flags
        } else {

            switch (letter) {
                case 'w': sideToMove = White; break;
                case 'b':
                    if (str.at(i+1) == '3') enPassantSq = B3;
                    else if (str.at(i+1) == '6') enPassantSq = B6;
                    else sideToMove = Black;
                    break;
                case 'k': castleFlags |= bShortCastle; break;
                case 'K': castleFlags |= wShortCastle; break;
                case 'q': castleFlags |= bLongCastle; break;
                case 'Q': castleFlags |= wLongCastle; break;
                case 'a': enPassantSq = (str[i + 1] == '3') ? A3 : A6; break;
                // b is handled separately, as it doubles as side t move
                case 'c': enPassantSq = (str[i + 1] == '3') ? C3 : C6; break;
                case 'd': enPassantSq = (str[i + 1] == '3') ? D3 : D6; break;
                case 'e': enPassantSq = (str[i + 1] == '3') ? E3 : E6; break;
                case 'f': enPassantSq = (str[i + 1] == '3') ? F3 : F6; break;
                case 'g': enPassantSq = (str[i + 1] == '3') ? G3 : G6; break;
                case 'h': enPassantSq = (str[i + 1] == '3') ? H3 : H6; break;
            }
        }
    }
    
    boardHash = CalculateHashKey();
    pawnKingHash = CalculatePawnKingKey();
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

Bitboard Position::CalculatePawnKingKey() {

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
    castleFlags &= Mask.castle[fromSquare] & Mask.castle[toSquare];
    boardHash ^= Key.castleKey[castleFlags];
}

void Position::TryMarkingIrreversible() {

    if (reversibleMoves == 0)
        repetitionIndex = 0;
}

std::tuple<Color, int> PieceFromChar(char c) {
    switch (c) {
    case 'P': return { White, Pawn };   case 'p': return { Black, Pawn };
    case 'N': return { White, Knight }; case 'n': return { Black, Knight };
    case 'B': return { White, Bishop };  case 'b': return { Black, Bishop };
    case 'R': return { White, Rook };   case 'r': return { Black, Rook };
    case 'Q': return { White, Queen }; case 'q': return { Black, Queen };
    case 'K': return { White, King };  case 'k': return { Black, King };
    default: return { colorNone, noPieceType };
    }
}
