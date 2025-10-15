// Publius - Didactic public domain bitboard chess engine 
// by Pawel Koziol

#pragma once

[[nodiscard]] constexpr int GetTypeOfMove(const Move move) {
    return move >> 12;
}

[[nodiscard]] constexpr Square GetFromSquare(const Move move) {
    return static_cast<Square>(move & 63);
}

[[nodiscard]] constexpr Square GetToSquare(const Move move) {
    return static_cast<Square>((move >> 6) & 63);
}

[[nodiscard]] constexpr PieceType GetPromotedPiece(const Move move) {
    return (PieceType)((move >> 12) - 3);
}

[[nodiscard]] constexpr Move CreateMove(const Square fromSquare,
    const Square toSquare,
    const int flag) {
    return (flag << 12) | (toSquare << 6) | fromSquare;
}

[[nodiscard]] constexpr bool IsMovePromotion(const Move move) {
    return move & 0x4000;
}

[[nodiscard]] bool IsMoveNoisy(Position* pos, const Move move);
[[nodiscard]] std::string MoveToString(const Move move);
[[nodiscard]] Move StringToMove(Position* pos, const std::string& moveString);

// Several functions need to transform raw move into some variables.
// Using MoveDescription struct is cleaner than setting separate
// variables.

struct MoveDescription {
    const Move      move;
    const Color     side;     // side to move in 'pos'
    const Square    fromSquare;
    const Square    toSquare;
    const PieceType hunter;   // piece on 'from' (may be noPieceType if illegal)
    const PieceType prey;     // piece on 'to'   (noPieceType if empty)
    const int       type;     // your MoveType (tNormal, tCastle, ...)

    explicit MoveDescription(const Position& pos, Move m) noexcept
        : move(m)
        , side(pos.GetSideToMove())
        , fromSquare(GetFromSquare(m))
        , toSquare(GetToSquare(m))
        , hunter(pos.PieceTypeOnSq(fromSquare))
        , prey(pos.PieceTypeOnSq(toSquare))
        , type(GetTypeOfMove(m))
    {}
};