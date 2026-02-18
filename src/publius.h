// Publius - Didactic public domain bitboard chess engine 
// by Pawel Koziol 

#pragma once

// fastchess.exe -openings order=random file=c:\fastchess\UHO_Lichess_4852_v1.epd -engine proto=uci name=new cmd=c:\fastchess\new.exe -engine proto=uci name=base cmd=c:\fastchess\old.exe -concurrency 3 -each tc=8+0.08 -rounds 100000 -repeat -recover -sprt alpha=0.05 beta=0.10 elo0=0 elo1=10

// non-regression
// fastchess.exe -openings order=random file=c:\fastchess\UHO_Lichess_4852_v1.epd -engine proto=uci name=new cmd=c:\fastchess\new.exe -engine proto=uci name=base cmd=c:\fastchess\old.exe -concurrency 3 -each tc=8+0.08 -rounds 100000 -repeat -recover -sprt alpha=0.05 beta=0.10 elo0=-10 elo1=0

// SPRT calculator: https://elocalculator.netlify.app/

//#define USE_TUNING

// REGEX to count all the lines under MSVC 13: ^(?([^\r\n])\s)*[^\s+?/]+[^\n]*$
// 4462 lines of source code

// r5k1/pQp2qpp/8/4pbN1/3P4/6P1/PPr4P/1K1R3R b - - 0 1 search explosion
// r2q1rk1/1b3p1p/p3pPp1/2ppP3/7R/1PN1B1R1/1PP2P1P/4K3 w - - 1 3  d 24, 30 s.
// r2q1r2/1b2bpkp/p3p1p1/2ppP1P1/7R/1PN1BQR1/1PP2P1P/4K3 w - - 0 1 1.Qf6, solved at depth 32 30 min

#define FAST_POPCNT
// uncomment this when compiling for modern Windows systems
// to gain a little speedup.

// #define HCE_ONLY
// turn that on if you want a version that uses only handcrafted
// evaluation function and does not load a NNUE

inline constexpr const char* engineName = "Publius";
inline constexpr const char* engineAuthor = "Pawel Koziol";
inline constexpr const char* engineVersion = "1.1.13"; // bump on releases
inline constexpr const char* netPath = "publius_net256_1.bin";
//inline constexpr const char* netPath = "publius_net32_4.bin";

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
extern int multiPv;

// entry points

void UciLoop(void);
void OnNewGame(void);
void Think(Position* pos);

// diagnostics

void Bench(Position* pos, int depth);
void PrintBoard(Position* pos);
Bitboard Perft(Position* pos, int ply, int depth, bool isNoisy);
void PrintBitboard(Bitboard b);