#include <iostream>
#include "types.h"
#include "limits.h"
#include "publius.h"
#include "move.h"
#include "pv.h"
#include "timer.h"

void PvCollector::Clear() {

    for (int i = 0; i < PlyLimit + 2; i++)
        for (int j = 0; j < PlyLimit + 2; j++)
            line[i][j] = 0;
}

void PvCollector::Update(const int ply, const Move move) {

    line[ply][ply] = move;

    for (int i = ply + 1; i < size[ply + 1]; ++i)
        line[ply][i] = line[ply + 1][i];

    size[ply] = size[ply + 1];
}

void PvCollector::SendBestMove() {

    if (line[0][1]) {
        // print best move and ponder move
        std::cout << "bestmove " << MoveToString(line[0][0])
                  << " ponder " << MoveToString(line[0][1])
                  << std::endl;
    }
    else {
        // print just best move
        std::cout << "bestmove " << MoveToString(line[0][0])
                  << std::endl;
    }
}

void PvCollector::Display(int score) {

    std::string scoreType;
    Bitboard nps = 0;
    int elapsed = Timer.Elapsed();

    // calculate nodes per second
    if (elapsed) nps = nodeCount * 1000 / elapsed;

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
    std::cout << "info depth " << rootDepth
              << " time " << elapsed
              << " nodes " << nodeCount
              << " nps " << nps
              << " score "
              << scoreType << " " << score << " pv";

    int lineLength = size[0]; // std::min(size[0], 1);

    for (int j = 0; j < lineLength; ++j) {
        std::cout << " " << MoveToString(line[0][j]);
    }
    std::cout << std::endl;
}