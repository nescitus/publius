// Publius - Didactic public domain bitboard chess engine 
// by Pawel Koziol

#pragma once

//#define USE_TUNING

// REGEX to count all the lines under MSVC 13: ^(?([^\r\n])\s)*[^\s+?/]+[^\n]*$
// 4251 lines

inline constexpr const char* engineName = "Publius";
inline constexpr const char* engineAuthor = "Pawel Koziol";
inline constexpr const char* engineVersion = "rc2"; // bump on releases
inline constexpr const char* netPath = "publius_net64_1.bin";
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