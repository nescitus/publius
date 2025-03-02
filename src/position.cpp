#include "color.h"
#include "square.h"
#include "publius.h"
#include "bitboard.h"
#include "bitgen.h"
#include "mask.h"
#include "hashkeys.h"
#include "piece.h"

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

void Position::Set(const std::string str) {

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
            key ^= Key.pieceKey[pieceLocation[s]][s];

    key ^= Key.castleKey[castleFlags];

    if (enPassantSq != sqNone)
        key ^= Key.enPassantKey[FileOf(enPassantSq)];

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
         boardHash ^= Key.enPassantKey[FileOf(enPassantSq)];
         enPassantSq = sqNone;
     }
}

void Position::MovePiece(const Color color, 
                         const int typeOfPiece, 
                         const Square fromSquare, Square toSquare) {

     pieceLocation[fromSquare] = noPiece;
     pieceLocation[toSquare] = CreatePiece(color, typeOfPiece);
     pieceBitboard[color][typeOfPiece] ^= Paint(fromSquare, toSquare);
}

void Position::TakePiece(const Color color, 
                         const int typeOfPiece, 
                         const Square square) {

     pieceLocation[square] = noPiece;
     pieceBitboard[color][typeOfPiece] ^= Paint(square);
     pieceCount[color][typeOfPiece]--;
}

void Position::AddPiece(const Color color, 
                        const int typeOfPiece, 
                        const Square square) {

     pieceLocation[square] = CreatePiece(color,typeOfPiece);
     pieceBitboard[color][typeOfPiece] ^= Paint(square);
     pieceCount[color][typeOfPiece]++;
}

void Position::ChangePiece(const int oldType, 
                           const int newType, 
                           const Color color, 
                           const Square square) {

     pieceLocation[square] = CreatePiece(color, newType);
     pieceBitboard[color][oldType] ^= Paint(square);
     pieceBitboard[color][newType] ^= Paint(square);
     pieceCount[color][newType]++;
     pieceCount[color][oldType]--;
}

bool Position::CanTryNullMove() {

     return ((CountMinors(sideToMove) + CountMajors(sideToMove)) > 0);
}

bool Position::IsDraw() {

    if (IsDrawBy50MoveRule())
        return true;

    if (IsDrawByRepetition())
        return true;

    if (IsDrawByInsufficientMaterial())
        return true;

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
        if (CountAllPawns() + CountMajors(White) + CountMajors(Black) == 0
        && CountMinors(White) + CountMinors(Black) <= 1) return true;
    }

    return false;
}

void Position::TryMarkingIrreversible() {
    if (reversibleMoves == 0)
        repetitionIndex = 0;
}

bool Position::IsInCheck() {
    return (SquareIsAttacked(KingSq(sideToMove), ~sideToMove) != 0);
}

bool Position::LeavesKingInCheck() {
    return (SquareIsAttacked(KingSq(~(sideToMove)), sideToMove) != 0);
}

bool Position::SquareIsAttacked(const Square sq, const Color color) {

	return (Map(color, Pawn)  & GenerateMoves.Pawn(~color, sq))
		|| (Map(color, Knight)  & GenerateMoves.Knight(sq))
		|| (MapDiagonalMovers(color) & GenerateMoves.Bish(Occupied(), sq))
		|| (MapStraightMovers(color) & GenerateMoves.Rook(Occupied(), sq))
		|| (Map(color, King)  & GenerateMoves.King(sq));
}

Color Position::GetSideToMove() {
    return sideToMove;
}

int Position::GetPiece(const Square square) {
    return pieceLocation[square];
}

int Position::CountMinors(const Color color) {
    return Count(color, Knight) + Count(color, Bishop);
}

int Position::CountMajors(const Color color) {
    return Count(color, Rook) + Count(color, Queen);
}

int Position::Count(const Color color, const int type) {
    return pieceCount[color][type];
}

int Position::CountAllPawns() {
    return Count(White, Pawn) + Count(Black, Pawn);
}

Bitboard Position::Map(const Color color, const int piece) {
    return (pieceBitboard[color][piece]);
}

Bitboard Position::Map(const Color color) {

    return Map(color, Pawn) | Map(color, Knight) | Map(color, Bishop)
         | Map(color, Rook) | Map(color, Queen) | Map(color, King);
}

Bitboard Position::Occupied() {
    return Map(White) | Map(Black);
}

Bitboard Position::Empty() {
    return ~Occupied();
}

Bitboard Position::MapDiagonalMovers(const Color color) {
    return (Map(color, Bishop) | Map(color, Queen));
}

Bitboard Position::MapStraightMovers(const Color color) {
    return (Map(color, Rook) | Map(color, Queen));
}

int Position::PieceTypeOnSq(const Square square) {
    return TypeOfPiece(pieceLocation[square]);
}

Square Position::KingSq(const Color color) {
    return kingSq[color];
}

Square Position::EnPassantSq() {
    return enPassantSq;
}

bool Position::IsEmpty(const Square sq) {
    return (Occupied() & Paint(sq)) == 0;
}

bool Position::IsPawnDefending(Color color, Square sq) {

    Bitboard defenders = (Map(White, Pawn) & GenerateMoves.Pawn(Black, sq)) |
                         (Map(Black, Pawn) & GenerateMoves.Pawn(White, sq));

    return ((defenders & Map(color, Pawn)) != 0);
}

Bitboard Position::MapPieceType(const int pieceType) {
    return pieceBitboard[White][pieceType] |
        pieceBitboard[Black][pieceType];
}

Bitboard Position::AllDiagMovers() {
    return MapPieceType(Bishop) | MapPieceType(Queen);
}

Bitboard Position::AllStraightMovers() {
    return MapPieceType(Rook) | MapPieceType(Queen);
}

Bitboard Position::AttacksTo(const Square sq) {

    return (Map(White, Pawn) & GenerateMoves.Pawn(Black, sq)) |
        (Map(Black, Pawn) & GenerateMoves.Pawn(White, sq)) |
        (MapPieceType(Knight) & GenerateMoves.Knight(sq)) |
        ((AllDiagMovers()) & GenerateMoves.Bish(Occupied(), sq)) |
        ((AllStraightMovers()) & GenerateMoves.Rook(Occupied(), sq)) |
        (MapPieceType(King) & GenerateMoves.King(sq));
}