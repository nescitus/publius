#include "types.h"
#include "limits.h"
#include "publius.h"
#include "move.h"
#include "pv.h"
#include <iostream>

void PvCollector::Clear() {

    for (int i = 0; i < PlyLimit + 2; i++)
        for (int j = 0; j < PlyLimit + 2; j++)
            line[i][j] = 0;
}

void PvCollector::Refresh(const int ply, const Move move) {

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

void PvCollector::PrintMainLine() {

    for (int j = 0; j < size[0]; ++j) {
        std::cout << " " << MoveToString(line[0][j]);
    }
}