// Publius - Didactic public domain bitboard chess engine 
// by Pawel Koziol

#pragma once

//#define USE_TUNING

// REGEX to count all the lines under MSVC 13: ^(?([^\r\n])\s)*[^\s+?/]+[^\n]*$
// 4365 lines

// r5k1/pQp2qpp/8/4pbN1/3P4/6P1/PPr4P/1K1R3R b - - 0 1 search explosion

// r2q1r2/1b2bpkp/p3p1p1/2ppP1P1/7R/1PN1BQR1/1PP2P1P/4K3 w - - 0 1 1.Qf6, unsolved

// Bench at depth 15 took 25622 milliseconds, searching 25859065 nodes at 1009252 nodes per second.

#define FAST_POPCNT
// uncomment this when compiling for modern Windows systems
// to gain a little speedup.

// #define HCE_ONLY
// turn that on if you want a version that uses only handcrafted
// evaluation function and does not load a NNUE

inline constexpr const char* engineName = "Publius";
inline constexpr const char* engineAuthor = "Pawel Koziol";
inline constexpr const char* engineVersion = "1.05"; // bump on releases
inline constexpr const char* netPath = "publius_net256_1.bin";

// Engine's name will contain information about unusual
// compile-time settings

#ifdef HCE_ONLY
    #ifdef FAST_POPCNT
    inline constexpr const char* compileParams = " HCE";
    #else
    inline constexpr const char* compileParams = " HCE slow";
    #endif
#else
    #ifdef FAST_POPCNT
    inline constexpr const char* compileParams = "";
    #else
    inline constexpr const char* compileParams = " slow";
    #endif
#endif

extern bool isNNUEloaded;
extern bool isUci;
extern int nnueWeight;
extern int hceWeight;

// entry points

void UciLoop(void);
void OnNewGame(void);
void Think(Position* pos);

// diagnostics

void Bench(Position* pos, int depth);
void PrintBoard(Position* pos);
Bitboard Perft(Position* pos, int ply, int depth, bool isNoisy);
void PrintBitboard(Bitboard b);