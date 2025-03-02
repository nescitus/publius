#include "limits.h"
#include "pv.h"

void PvCollector::Clear() {

    for (int i = 0; i < PlyLimit + 2; i++)
        for (int j = 0; j < PlyLimit + 2; j++)
            line[i][j] = 0;
}

void PvCollector::Refresh(int ply, int move) {

    line[ply][ply] = move;

    for (int j = ply + 1; j < size[ply + 1]; ++j)
        line[ply][j] = line[ply + 1][j];

    size[ply] = size[ply + 1];
}