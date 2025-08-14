// Publius - Didactic public domain bitboard chess engine by Pawel Koziol

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

[[nodiscard]] constexpr int GetPromotedPiece(const Move move) {
    return (move >> 12) - 3;
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