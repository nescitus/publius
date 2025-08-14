// Publius - Didactic public domain bitboard chess engine by Pawel Koziol

#pragma once

#ifdef USE_TUNING

struct Sample {
    std::string epd;  // full EPD/FEN line
    double result;    // 1.0 (1-0), 0.0 (0-1), 0.5 (1/2-1/2)
};

#include <vector>
#include <cstdint>
#include <cinttypes>
#include <string>
#include <sstream>     // std::getline with std::istringstream
#include <fstream>     // std::ifstream

class Parameters; // forward declaration needed for pointer/reference to parameters

class cTuner {
public:
    std::vector<Sample> dataset;
    void Init(int filter);
    double TexelFit(Position* p);
    double TuneSingleSquare(Position* pos, Parameters* params, int piece, Square s, int delta, double currentFit);
    double TryChangeMgPst(Position* pos, Parameters* params, int piece, Square sq, int delta, double baselineLoss);
    double TryChangeEgPst(Position* pos, Parameters* params, int piece, Square sq, int delta, double baselineLoss);
};

extern cTuner Tuner;

#endif