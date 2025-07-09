#include "types.h"
#include "square.h"
#include "publius.h"
#include "bitboard.h"
#include "move.h"
#include "timer.h"
#include "movepicker.h"
#include "search.h"

std::string test[] = {
 "r1bqkbnr/pp1ppppp/2n5/2p5/4P3/5N2/PPPP1PPP/RNBQKB1R w KQkq -",           // 1.e4 c5 2.Nf3 Nc6
 "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -",       // multiple captures
 "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -",                                  // rook endgame
 "r3kbbr/pp1n1p1P/3ppnp1/q5N1/1P1pP3/P1N1B3/2P1QP2/R3KB1R b KQkq b3 0 17", // promotions
 "rq3rk1/ppp2ppp/1bnpb3/3N2B1/3NP3/7P/PPPQ1PP1/2KR3R w - - 7 14",          // knight pseudo-sack
 "r1bq1r1k/1pp1n1pp/1p1p4/4p2Q/4Pp2/1BNP4/PPP2PPP/3R1RK1 w - - 2 14",      // pawn chain
 "2rr2k1/1p4bp/p1q1p1p1/4Pp1n/2PB4/1PN3P1/P3Q2P/2RR2K1 w - f6 0 20",       // possible en passant        
 "r1bbk1nr/pp3p1p/2n5/1N4p1/2Np1B2/8/PPP2PPP/2KR1B1R w kq - 0 13",
 "r1bq1rk1/ppp1nppp/4n3/3p3Q/3P4/1BP1B3/PP1N2PP/R4RK1 w - - 1 16",         // attack for pawn
 "4r1k1/r1q2ppp/ppp2n2/4P3/5Rb1/1N1BQ3/PPP3PP/R5K1 w - - 1 17",            // exchange sack
 "2rqkb1r/ppp2p2/2npb1p1/1N1Nn2p/2P1PP2/8/PP2B1PP/R1BQK2R b KQ - 0 11",
 "r1bq1r1k/b1p1npp1/p2p3p/1p6/3PP3/1B2NN2/PP3PPP/R2Q1RK1 w - - 1 16",      // white pawn center
 "3r1rk1/p5pp/bpp1pp2/8/q1PP1P2/b3P3/P2NQRPP/1R2B1K1 b - - 6 22",
 "r1q2rk1/2p1bppp/2Pp4/p6b/Q1PNp3/4B3/PP1R1PPP/2K4R w - - 2 18",
 "4k2r/1pb2ppp/1p2p3/1R1p4/3P4/2r1PN2/P4PPP/1R4K1 b - - 3 22",             // endgame
 "3q2k1/pb3p1p/4pbp1/2r5/PpN2N2/1P2P2P/5PP1/Q2R2K1 b - - 4 26",            // both queens en prise
 "1r2r2k/1b4q1/pp5p/2pPp1p1/P3Pn2/1P1B1Q1P/2R3P1/4BR1K b - - 1 37",       // closed middlegame
 "8/k7/3p4/p2P1p2/P2P1P2/8/8/K7 w - -",                                   // Fine # 70
};

// Bench function runs the analysis of several positions
// to the desired depth. It has two uses:
// - confirming that a change is indeed non-functional
// - measuring the impact of a change to the engine's speed

void Bench(Position* pos, int depth) {

    nodeCount = 0;
    State.isStopping = false;
    Timer.SetStartTime();
    Timer.SetData(maxDepth, depth);
    Timer.SetData(moveTime, 3600000); // more than one hour of benchmark would be useless ;)
    Timer.SetData(isInfinite, 1);

    for (int i = 0; i < std::size(test); ++i) {

        std::cout << test[i] << std::endl;
        OnNewGame();
        pos->Set(test[i]);
        Iterate(pos);
    }

    int elapsed = Timer.Elapsed();
    Bitboard nps = 0;
    if (elapsed) nps = nodeCount * 1000 / elapsed;

    std::cout << "Bench at depth " << depth
              << " took " << elapsed << " milliseconds, searching "
              << nodeCount << " nodes at " << nps << " nodes per second."
              << std::endl;
}

// print board
void PrintBoard(Position* pos) {

    const std::string piece_name[] = { "P ", "p ", "N ", "n ", "B ", "b ", "R ", "r ", "Q ", "q ", "K ", "k ", ". " };

    // print horizontal line (and spaces before the first square)
    std::cout << "--------------------------------------------" << std::endl << "  ";

    for (Square square = A1; square < 64; ++square) {

        // print square content
        Square mappedSquare = InvertSquare(square);
        std::cout << piece_name[pos->GetPiece(mappedSquare)];

        // print numbers and start new line with spaces
        if ((square + 1) % 8 == 0) {
            std::cout << "  " << 9 - ((square + 1) / 8) << std::endl << "  ";
        }
    }
    // print letters
    std::cout << std::endl << "  a b c d e f g h" << std::endl << std::endl;
    
    // print horizontal line
    std::cout << "--------------------------------------------" << std::endl;
}

// Perft can be used to measure move generation and board update
// speed, but more importantly, to confirm that the move generator
// works correctly. This version comes with a "divide" enhancement,
// printing out subtotals for each move.
Bitboard Perft(Position* pos, int ply, int depth, bool isNoisy) {

    Move move = 0;
    Bitboard localCount = 0;
    Bitboard moveCount = 0;
    UndoData undo;
    MovePicker movePicker;

    movePicker.Init(0);

    while ((move = movePicker.NextMove(pos, ply, modeAll)) != 0) {

        pos->DoMove(move, &undo);

        if (pos->LeavesKingInCheck()) {
            pos->UndoMove(move, &undo);
            continue;
        }

        localCount = (depth == 1) ? 1 
                                  : Perft(pos, ply + 1, depth - 1, isNoisy);
        moveCount += localCount;

        pos->UndoMove(move, &undo);

        if (ply == 0 && isNoisy)
            std::cout << MoveToString(move) << ": " << localCount << std::endl;
        }
    
    return moveCount;
}

void PrintBitboard(Bitboard b) {
   
    for (Square square = A1; square < 64; ++square) {
        
        if (b & RelSqBb(Black, square)) std::cout << "+ ";
        else                            std::cout << ". ";

        if ((square + 1) % 8 == 0) 
            std::cout << 9 - ((square + 1) / 8) << std:: endl;
    }

    std::cout  << "\na b c d e f g h" << std::endl;
}