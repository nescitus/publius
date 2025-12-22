// Publius - Didactic public domain bitboard chess engine 
// by Pawel Koziol

#pragma once

class EvalData {
public:
    int score[2];
    int pawnScore[2];
    Bitboard enemyKingZone[2];
    int kingAttUnits[2];
    Bitboard control[2][6];
    Bitboard allAtt[2];
    int gamePhase; // game phase (24 for starting position)
    void Clear();
    void Add(Color color, int val);
    void AddPawn(Color color, int val);
    void AddAttacks(Color color, Bitboard att, int strong, int weak);
};