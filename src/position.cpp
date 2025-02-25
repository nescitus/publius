#include "publius.h"
#include "bitboard.h"
#include "bitgen.h"
#include "mask.h"
#include "piece.h"
#include "square.h"

void Position::Clear() {

	for (Color color = White; color < colorNone; ++color) {
		
        kingSq[color] = sqNone;
	
        for (int type = 0; type < 6; type++) {
			pieceBitboard[color][type] = 0ULL;
			pieceCount[color][type] = 0;
		}
	}

    for (Square square = A1; square < 64; ++square) {
        pieceLocation[square] = noPiece;
    }

	castleFlags = 0;
	reversibleMoves = 0;
	repetitionIndex = 0;
	enPassantSq = sqNone;
	sideToMove = White;
}

void Position::Set(std::string str) {

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
                    AddPiece(Black, Pawn, InvertSquare(square)); 
                    ++square; 
                    break;
				case 'b': 
                    AddPiece(Black, Bishop, InvertSquare(square)); 
                    ++square; 
                    break;
				case 'n': 
                    AddPiece(Black, Knight, InvertSquare(square)); 
                    ++square; 
                    break;
				case 'r': 
                    AddPiece(Black, Rook, InvertSquare(square)); 
                    ++square; 
                    break;
				case 'q': 
                    AddPiece(Black, Queen, InvertSquare(square)); 
                    ++square; 
                    break;
				case 'k': 
                    AddPiece(Black, King, InvertSquare(square)); 
                    kingSq[Black] = InvertSquare(square); 
                    ++square; 
                    break;
				case 'P': 
                    AddPiece(White, Pawn, InvertSquare(square)); 
                    ++square; 
                    break;
				case 'B': 
                    AddPiece(White, Bishop, InvertSquare(square)); 
                    ++square; 
                    break;
				case 'N': 
                    AddPiece(White, Knight, InvertSquare(square)); 
                    ++square; 
                    break;
				case 'R': 
                    AddPiece(White, Rook, InvertSquare(square)); 
                    ++square; 
                    break;
				case 'Q': 
                    AddPiece(White, Queen, InvertSquare(square)); 
                    ++square; 
                    break;
				case 'K': 
                    AddPiece(White, King, InvertSquare(square)); 
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
}

Bitboard Position::CalculateHashKey() {

    Bitboard key = 0;

    for (Square s = A1; s < 64; ++s)
        if (pieceLocation[s] != noPiece)
            key ^= Mask.pieceKey[pieceLocation[s]][s];

    key ^= Mask.castleKey[castleFlags];

    if (enPassantSq != sqNone)
        key ^= Mask.enPassantKey[FileOf(enPassantSq)];

    if (sideToMove == Black)
        key ^= sideRandom;

    return key;
}

void Position::SwitchSide() {

     sideToMove = ~sideToMove;
     boardHash ^= sideRandom;
}

void Position::ClearEnPassant() {

     if (enPassantSq != sqNone) {
         boardHash ^= Mask.enPassantKey[FileOf(enPassantSq)];
         enPassantSq = sqNone;
     }
}

void Position::MovePiece(Color color, int typeOfPiece, Square fromSquare, Square toSquare) {

     pieceLocation[fromSquare] = noPiece;
     pieceLocation[toSquare] = CreatePiece(color, typeOfPiece);
     pieceBitboard[color][typeOfPiece] ^= Paint(fromSquare, toSquare);
}

void Position::TakePiece(Color color, int typeOfPiece, Square square) {

     pieceLocation[square] = noPiece;
     pieceBitboard[color][typeOfPiece] ^= Paint(square);
     pieceCount[color][typeOfPiece]--;
}

void Position::AddPiece(Color color, int typeOfPiece, Square square) {

     pieceLocation[square] = CreatePiece(color,typeOfPiece);
     pieceBitboard[color][typeOfPiece] ^= Paint(square);
     pieceCount[color][typeOfPiece]++;
}

void Position::ChangePiece(int oldType, int newType, Color color, Square square) {

     pieceLocation[square] = CreatePiece(color, newType);
     pieceBitboard[color][oldType] ^= Paint(square);
     pieceBitboard[color][newType] ^= Paint(square);
     pieceCount[color][newType]++;
     pieceCount[color][oldType]--;
}

bool Position::IsNullMoveOk() {

     return ((GetMinorCount(sideToMove) + GetMajorCount(sideToMove)) > 0);
}

bool Position::IsDraw() {

    if (IsDrawBy50MoveRule()) {
        return true;
    }

    if (IsDrawByRepetition()) {
        return true;
    }

    if (IsDrawByInsufficientMaterial()) {
        return true;
    }

	return false;
}

bool Position::IsDrawBy50MoveRule() {
    return (reversibleMoves > 100);
}

bool Position::IsDrawByRepetition() {

    for (int i = 4; i <= reversibleMoves; i += 2) {
        if (boardHash == repetitionList[repetitionIndex - i]) {
            return true;
        }
    }

    return false;
}

bool Position::IsDrawByInsufficientMaterial() {

    if (!LeavesKingInCheck()) {
        if (GetAllPawnsCount() + GetMajorCount(White) + GetMajorCount(Black) == 0
        && GetMinorCount(White) + GetMinorCount(Black) <= 1) return true;
    }

    return false;
}

void Position::TryMarkingIrreversible() {

    if (reversibleMoves == 0) {
        repetitionIndex = 0;
    }
}

bool Position::IsInCheck() {
    return (SquareIsAttacked(KingSq(sideToMove), ~sideToMove));
}

bool Position::LeavesKingInCheck() {
    return (SquareIsAttacked(KingSq(~(sideToMove)), sideToMove) != 0);
}

int Position::SquareIsAttacked(Square square, Color color) {

	return (Map(color, Pawn)  & GenerateMoves.Pawn(~color, square))
		|| (Map(color, Knight)  & GenerateMoves.Knight(square))
		|| (MapDiagonalMovers(color) & GenerateMoves.Bish(Occupied(), square))
		|| (MapStraightMovers(color) & GenerateMoves.Rook(Occupied(), square))
		|| (Map(color, King)  & GenerateMoves.King(square));
}

Color Position::GetSide() {
    return sideToMove;
}

int Position::GetPiece(Square square) {
    return pieceLocation[square];
}

int Position::GetMinorCount(Color color) {
    return GetCount(color, Knight) + GetCount(color, Bishop);
}

int Position::GetMajorCount(Color color) {
    return GetCount(color, Rook) + GetCount(color, Queen);
}

int Position::GetCount(Color color, int type) {
    return pieceCount[color][type];
}

int Position::GetAllPawnsCount() {
    return GetCount(White, Pawn) + GetCount(Black, Pawn);
}

Bitboard Position::Map(Color color, int piece) {
    return (pieceBitboard[color][piece]);
}

Bitboard Position::Map(Color color) {

    return Map(color, Pawn) | Map(color, Knight) | Map(color, Bishop)
         | Map(color, Rook) | Map(color, Queen) | Map(color, King);
}

Bitboard Position::Occupied() {
    return Map(White) | Map(Black);
}

Bitboard Position::Empty() {
    return ~Occupied();
}

Bitboard Position::MapDiagonalMovers(Color color) {
    return (Map(color, Bishop) | Map(color, Queen));
}

Bitboard Position::MapStraightMovers(Color color) {
    return (Map(color, Rook) | Map(color, Queen));
}

int Position::PieceTypeOnSq(Square square) {
    return TypeOfPiece(pieceLocation[square]);
}

Square Position::KingSq(Color color) {
    return kingSq[color];
}

Square Position::EnPassantSq() {
    return enPassantSq;
}

bool Position::IsEmpty(Square sq) {
    return (Occupied() & (Paint(sq) == 0));
}
