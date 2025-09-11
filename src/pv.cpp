// Publius - Didactic public domain bitboard chess engine 
// by Pawel Koziol

// This class implements triangular pv-table
// (https://www.chessprogramming.org/Triangular_PV-Table)
// that remembers the main line calculated by the engine.

#include <iostream>
#include "types.h"
#include "limits.h"
#include "position.h"
#include "move.h"
#include "pv.h"
#include "timer.h"

// Clears pv-array
void PvCollector::Clear(const Move ttMove) {

    for (int i = 0; i < PlyLimit + 2; i++)
        for (int j = 0; j < PlyLimit + 2; j++)
            line[i][j] = 0;

    line[0][0] = ttMove;
}

// Updates pv-array
void PvCollector::Update(const int ply, const Move move) {

    line[ply][ply] = move;

    for (int i = ply + 1; i < size[ply + 1]; ++i)
        line[ply][i] = line[ply + 1][i];

    size[ply] = size[ply + 1];
}

// Sends best move (and ponder move if present)
void PvCollector::SendBestMove() {

    if (line[0][1]) {
        // print best move and ponder move
        std::cout << "bestmove " << MoveToString(line[0][0])
                  << " ponder " << MoveToString(line[0][1])
                  << std::endl;
    } else {
        // print just best move
        std::cout << "bestmove " << MoveToString(line[0][0])
                  << std::endl;
    }
}

// Displays engine's main line
void PvCollector::Display(int score) {

    std::string scoreType;
    
    Timer.RefreshStats();

    // If we are outside of normal evaluation range,
    // then the engine either gives a checkmate
    // or is being mated. In this case, we translate
    // the score into distance to mate and set
    // approppriate score type ("mate" instead of
    // the usual centipawns)
    scoreType = "mate";
    if (score < -EvalLimit)
        score = (-MateScore - score) / 2;
    else if (score > EvalLimit)
        score = (MateScore - score + 1) / 2;
    else scoreType = "cp";

    // print statistics
    std::cout << "info depth " << Timer.rootDepth
              << " time " << Timer.timeUsed
              << " nodes " << Timer.nodeCount
              << " nps " << Timer.nps
              << " score "
              << scoreType << " " << score << " pv";

    for (int j = 0; j < size[0]; ++j)
        std::cout << " " << MoveToString(line[0][j]);

    std::cout << std::endl;
}