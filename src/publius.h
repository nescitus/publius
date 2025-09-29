// Publius - Didactic public domain bitboard chess engine 
// by Pawel Koziol

#pragma once

//#define USE_TUNING

// REGEX to count all the lines under MSVC 13: ^(?([^\r\n])\s)*[^\s+?/]+[^\n]*$
// 4308 lines

// r5k1/pQp2qpp/8/4pbN1/3P4/6P1/PPr4P/1K1R3R b - - 0 1 search explosion

// r2q1r2/1b2bpkp/p3p1p1/2ppP1P1/7R/1PN1BQR1/1PP2P1P/4K3 w - - 0 1

inline constexpr const char* engineName = "Publius";
inline constexpr const char* engineAuthor = "Pawel Koziol";
inline constexpr const char* engineVersion = "rc2"; // bump on releases
inline constexpr const char* netPath = "publius_net96_1.bin"; 
extern bool isNNUEloaded;
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