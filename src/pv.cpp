#include "limits.h"
#include "pv.h"
#include <iostream>

void PvCollector::Clear() {

    for (int i = 0; i < PlyLimit + 2; i++)
        for (int j = 0; j < PlyLimit + 2; j++)
            line[i][j] = 0;
}

void PvCollector::Refresh(const int ply, const int move) {

    line[ply][ply] = move;

    for (int i = ply + 1; i < size[ply + 1]; ++i)
        line[ply][i] = line[ply + 1][i];

    size[ply] = size[ply + 1];
}