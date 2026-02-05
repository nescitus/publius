// Publius - Didactic public domain bitboard chess engine 
// by Pawel Koziol

#pragma once
#include <string>

// data for undoing a move

typedef struct {
    Move move;
    PieceType prey;
    int castleFlags;
    Square enPassantSq;
    int reversibleMoves;
    Bitboard boardHash;
    Bitboard pawnHash;
} UndoData;

// Position class: stores the board state, makes 
// and unmakes moves, provides information about 
// the current position.

// [[nodiscard]] is a compiler hint (C++17+) meaning:
// "Warn if the return value of this function is ignored."

class Position {
private:
    // --- Core state ---
    Color sideToMove;
    Bitboard pieceBitboard[2][6];
    Square kingSq[2];
    Square enPassantSq;
    int castleFlags;
    int pieceCount[2][6];
    ColoredPiece pieceLocation[64];
    int reversibleMoves;
    int repetitionIndex;
    Bitboard repetitionList[256];

    // --- Internal helpers (not for public use) ---
    void Clear();
    void ClearEnPassant();
    void MovePiece(Color color, PieceType hunter, Square from, Square to);
    void MovePieceNoHash(Color color, PieceType hunter, Square from, Square to);
    void AddPieceNoHash(Color color, PieceType type, Square sq);
    void TakePiece(Color color, PieceType type, Square sq);
    void TakePieceNoHash(Color color, int type, Square sq);
    void ChangePieceNoHash(PieceType oldType, PieceType newType, Color color, Square sq);
    void SetEnPassantSquare(Color color, Square to);
    void UpdateCastlingRights(Square from, Square to);
    Bitboard CalculateHashKey();
    Bitboard CalculatePawnKingKey();
    [[nodiscard]] bool IsDrawByRepetition() const;
    [[nodiscard]] bool IsDrawByInsufficientMaterial() const;
    void TrySettingEp(char numberChar, Square whiteSq, Square blackSq);

public:

    // --- Zobrist hashes ---
    Bitboard boardHash;  // full position hash (side to move, pieces, castling, ep)
    Bitboard pawnKingHash; // hash key for pawn + king structure (for faster pawn eval)

    // --- Move execution ---
    void Set(const std::string& fen);
    void DoMove(Move move, UndoData* undo);
    void DoNull(UndoData* undo);
    void UndoMove(Move move, UndoData* undo);
    void UndoNull(UndoData* undo);
    void TryMarkingIrreversible();

    // --- Game state queries ---
    [[nodiscard]] bool IsDraw() const;
    [[nodiscard]] bool CanTryNullMove() const;
    [[nodiscard]] bool IsInCheck() const;
    [[nodiscard]] bool IsOwnKingInCheck() const; // illegal position
    [[nodiscard]] bool IsWhiteShortCastleLegal();
    [[nodiscard]] bool IsWhiteLongCastleLegal();
    [[nodiscard]] bool IsBlackShortCastleLegal();
    [[nodiscard]] bool IsBlackLongCastleLegal();

    // --- Piece counting ---
    [[nodiscard]] int CountMinors(const Color color) const {
        return Count(color, Knight) + Count(color, Bishop);
    }

    [[nodiscard]] int CountMajors(const Color color) const {
        return Count(color, Rook) + Count(color, Queen);
    }

    [[nodiscard]] int CountAllPawns() const {
        return Count(White, Pawn) + Count(Black, Pawn);
    }

    [[nodiscard]] int Count(Color color, int type) const {
        return pieceCount[color][type];
    }

    // --- Board maps ---
    [[nodiscard]] Bitboard Map(Color color, PieceType piece) const {
        return pieceBitboard[color][piece];
    }
    [[nodiscard]] Bitboard Pieces(Color color) const;
    [[nodiscard]] Bitboard MapDiagonalSliders(Color color) const;
    [[nodiscard]] Bitboard MapStraightSliders(Color color) const;
    [[nodiscard]] Bitboard AllDiagMovers() const;
    [[nodiscard]] Bitboard AllStraightMovers() const;
    [[nodiscard]] Bitboard Occupied() const;
    [[nodiscard]] Bitboard Empty() const;
    [[nodiscard]] Bitboard MapPieceType(const PieceType pieceType) const {
        return pieceBitboard[White][pieceType] |
               pieceBitboard[Black][pieceType];
    }

    // --- Attack detection ---
    bool Position::EitherSquareIsAttacked(const Square s1, const Square s2, const Color color) const;
    [[nodiscard]] bool SquareIsAttacked(Square sq, Color byColor) const;
    [[nodiscard]] Bitboard AttacksTo(Square sq) const;
    [[nodiscard]] Bitboard AttacksFrom(Square sq) const;
    [[nodiscard]] bool MoveGivesCheck(Move move);

    // --- Basic getters (fast, inline) ---
    [[nodiscard]] Color GetSideToMove() const { return sideToMove; }
    [[nodiscard]] ColoredPiece GetPiece(Square sq) const { return pieceLocation[sq]; }
    [[nodiscard]] bool IsOccupied(Square sq) const { return pieceLocation[sq] != noPiece; }
    [[nodiscard]] Square KingSq(Color color) const { return kingSq[color]; }
    [[nodiscard]] Square EnPassantSq() const { return enPassantSq; }
    [[nodiscard]] PieceType PieceTypeOnSq(Square sq) const;
};

// this helps keeping Position::Set() more concise
std::tuple<Color, PieceType> PieceFromChar(char c);
