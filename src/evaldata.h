// Publius - Didactic public domain bitboard chess engine by Pawel Koziol

#pragma once

class EvalData {
public:
    int mg[2];
    int eg[2];
    int mgPawn[2];
    int egPawn[2];
    Bitboard enemyKingZone[2];
    int kingAttUnits[2];
    Bitboard control[2][6];
    Bitboard allAtt[2];
    int phase; // game phase (24 for starting position)
    void Clear();
    void Add(Color color, int mgVal, int egVal);
    void AddPawn(Color color, int mgVal, int egVal);
};