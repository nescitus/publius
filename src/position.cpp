// Publius - Didactic public domain bitboard chess engine 
// by Pawel Koziol

#include <tuple>
#include <cctype> // isdigit
#include "types.h"
#include "piece.h"
#include "square.h"
#include "mask.h"
#include "nn.h"
#include "position.h"
#include "bitboard.h"
#include "bitgen.h"
#include "hashkeys.h"
#include "publius.h" // for hasNNUE

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

            // on finding a number, update square
            if (std::isdigit(letter)) {
                square = square + (int)(letter - '0');
            // if it's a letter, it denotes a piece
            } else {
                auto tuple = PieceFromChar(letter);
                Color color = std::get<0>(tuple);
                PieceType pieceType = std::get<1>(tuple);

                if (color != colorNone) {
                    AddPieceNoHash(color, pieceType, MirrorRank(square));
                    if (pieceType == King)
                        kingSq[color] = MirrorRank(square);
                    ++square;
                }
            }

        // The second loop sets flags
        } else {

            switch (letter) {
            case 'w': sideToMove = White; break;
            case 'b':
                if (str.at(i + 1) == '3') enPassantSq = B3;
                else if (str.at(i + 1) == '6') enPassantSq = B6;
                else sideToMove = Black;
                break;
            case 'k': castleFlags |= bShortCastle; break;
            case 'K': castleFlags |= wShortCastle; break;
            case 'q': castleFlags |= bLongCastle; break;
            case 'Q': castleFlags |= wLongCastle; break;
            case 'a': TrySettingEp(str[i + 1], A3, A6); break;
            // 'b' is handled separately, since it doubles as side t move
            case 'c': TrySettingEp(str[i + 1], C3, C6); break;
            case 'd': TrySettingEp(str[i + 1], D3, D6); break;
            case 'e': TrySettingEp(str[i + 1], E3, E6); break;
            case 'f': TrySettingEp(str[i + 1], F3, F6); break;
            case 'g': TrySettingEp(str[i + 1], G3, G6); break;
            case 'h': TrySettingEp(str[i + 1], H3, H6); break;
            }
        }
    }

    boardHash = CalculateHashKey();
    pawnKingHash = CalculatePawnKingKey();
    NN.Refresh(*this);
}

void Position::TrySettingEp(char number, Square whiteSq, Square blackSq) {
    if (Is3or6(number)) 
        enPassantSq = (number == '3') ? whiteSq : blackSq;
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

void Position::ClearEnPassant() {

    if (enPassantSq != sqNone) {
        boardHash ^= Key.enPassantKey[FileOf(enPassantSq)];
        enPassantSq = sqNone;
    }
}

void Position::MovePiece(const Color color, const PieceType pieceType,
    const Square fromSquare, const Square toSquare) {

    MovePieceNoHash(color, pieceType, fromSquare, toSquare);
    boardHash ^= Key.ForPiece(color, pieceType, fromSquare) ^
        Key.ForPiece(color, pieceType, toSquare);
}

void Position::MovePieceNoHash(const Color color, const PieceType pieceType,
    const Square fromSquare, Square toSquare) {

    pieceLocation[fromSquare] = noPiece;
    pieceLocation[toSquare] = CreatePiece(color, pieceType);
    pieceBitboard[color][pieceType] ^= Paint(fromSquare, toSquare);

    if (isNNUEloaded)
        NN.Move(color, pieceType, toSquare, fromSquare);
}

void Position::TakePiece(const Color color,
    const PieceType pieceType,
    const Square square) {

    TakePieceNoHash(color, pieceType, square);
    boardHash ^= Key.ForPiece(color, pieceType, square);
}

void Position::TakePieceNoHash(const Color color,
    const int pieceType,
    const Square square) {

    pieceLocation[square] = noPiece;
    pieceBitboard[color][pieceType] ^= Paint(square);
    pieceCount[color][pieceType]--;

    if (isNNUEloaded)
        NN.Del(color, pieceType, square);
}

void Position::AddPieceNoHash(const Color color,
    const PieceType pieceType,
    const Square square) {

    pieceLocation[square] = CreatePiece(color, pieceType);
    pieceBitboard[color][pieceType] ^= Paint(square);
    pieceCount[color][pieceType]++;

    if (isNNUEloaded)
        NN.Add(color, pieceType, square);
}

void Position::ChangePieceNoHash(const PieceType oldType,
    const PieceType newType,
    const Color color,
    const Square square) {

    pieceLocation[square] = CreatePiece(color, newType);
    pieceBitboard[color][oldType] ^= Paint(square);
    pieceBitboard[color][newType] ^= Paint(square);
    pieceCount[color][newType]++;
    pieceCount[color][oldType]--;

    if (isNNUEloaded) {
        NN.Del(color, oldType, square);
        NN.Add(color, newType, square);
    }
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

std::tuple<Color, PieceType> PieceFromChar(char c) {
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
