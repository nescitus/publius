// Publius - Didactic public domain bitboard chess engine 
// by Pawel Koziol

// This class implements triangular pv-table
// (https://www.chessprogramming.org/Triangular_PV-Table)
// that remembers the main line calculated by the engine.
// We build various display functions around that. 

#include <iostream>
#include "types.h"
#include "limits.h"
#include "position.h"
#include "move.h"
#include "trans.h" // for bounds
#include "pv.h"
#include "timer.h"

// Clears pv-array
void PvCollector::Clear() {

    for (int i = 0; i < SearchTreeSize + 2; i++)
        size[i] = 0;

    for (int i = 0; i < SearchTreeSize + 2; i++)
        for (int j = 0; j < SearchTreeSize + 2; j++)
            line[i][j] = 0;
}

// Updates pv-array
void PvCollector::Update(const int ply, const Move move) {

    line[ply][ply] = move;

    for (int i = ply + 1; i < size[ply + 1]; ++i)
        line[ply][i] = line[ply + 1][i];

    size[ply] = size[ply + 1];
}

// Returns best move
Move PvCollector::GetBestMove() {
    return line[0][0];
}

// Remembers best line. We use it for simplifying
// upperbound message display. Displaying just the first
// move would be more rational, but unfortunately if we 
// do it, Arena hides the line instead of printing it.
void PvCollector::RememberBestLine() {
    oldPvString = GetPvString();
}

// Overwrites best move with a move from the external
// source, resets ponder move
void PvCollector::Overwrite(Move move) {
    line[0][0] = move;
    line[0][1] = 0; // no ponder move
}

// Sends best move (and ponder move if present)
void PvCollector::SendBestMove() {

    if (line[0][1]) {
        // print best move and ponder move
        std::cout << "bestmove " << MoveToString(line[0][0])
                  << " ponder " << MoveToString(line[0][1])
                  << "\n" << std::flush;
    } else {
        // print just best move
        std::cout << "bestmove " << MoveToString(line[0][0])
                  << "\n" << std::flush;
    }
}

// Displays engine's main line during non-multipv search
void PvCollector::Display(int score, int bound) {
    
    std::cout << "info depth " << Timer.rootDepth
              << GetOutputStringWithoutDepth(score, bound) << std::flush;
}

// get the main chunk of the string to display. A prefix will be
// added, depending on whether we are doing a multiPv search.
std::string PvCollector::GetOutputStringWithoutDepth(int score, int bound) {

    Timer.RefreshStats();

    return GetTimeString()
         + GetScoreString(score, bound)
         + (bound == upperBound ? oldPvString : GetPvString()) 
        + "\n";
}

// Get substring with time and engine speed data
std::string PvCollector::GetTimeString() {

        return " time " + std::to_string(Timer.timeUsed)
         + " nodes " + std::to_string(Timer.nodeCount)
         + " nps " + std::to_string(Timer.nps);
}

// Gets score, score type and bound substring
std::string PvCollector::GetScoreString(int score, int bound) {

    // If we are outside of normal evaluation range,
    // then the engine either gives a checkmate
    // or is being mated. In this case, we translate
    // the score into distance to mate and set
    // approppriate score type ("mate" instead of
    // the usual centipawns)

    std::string scoreType = " score mate";
    if (score < -EvalLimit)
        score = (-MateScore - score) / 2;
    else if (score > EvalLimit)
        score = (MateScore - score + 1) / 2;
    else scoreType = " score cp";

    std::string scoreBound = "";
    if (bound == lowerBound) scoreBound = " lowerbound";
    if (bound == upperBound) scoreBound = " upperbound";

    return scoreType + " " + std::to_string(score) + scoreBound;
}

// Gets mainline move sequence
std::string PvCollector::GetPvString() {

    std::string result = " pv";
    
    for (int j = 0; j < size[0]; ++j)
        result += " " + MoveToString(line[0][j]);

    return result;
}