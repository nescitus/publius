// Publius - Didactic public domain bitboard chess engine 
// by Pawel Koziol

#include <stdio.h>
#include <stdlib.h>
#include <climits>
#include <iostream>
#include <sstream>
#include "types.h"
#include "limits.h"
#include "position.h"
#include "publius.h"
#include "timer.h"
#include "history.h"
#include "trans.h"
#include "move.h"
#include "score.h"
#include "evaldata.h"
#include "pv.h"
#include "uci.h"
#include "params.h"
#include "tuner.h"
#include "api.h"
#include "nn.h"

#ifdef USE_TUNING
   cTuner Tuner;
#endif

void UciLoop(void) {

    Position pos[1];
    pos->Set(startFen);
    TT.Allocate(16);

    std::string line;
    while (getline(std::cin, line)) {
        std::istringstream ss(line);
        if (!ParseCommand(ss, pos)) 
            break;
    }
}

bool ParseCommand(std::istringstream& stream, Position* pos) {

    std::string command;
    stream >> command;

    if (command == "isready") std::cout << "readyok" << std::endl;
    else if (command == "ucinewgame") OnNewGame();
    else if (command == "uci") OnUciCommand();
    else if (command == "position") OnPositionCommand(stream, pos);
    else if (command == "go") OnGoCommand(stream, pos);
    else if (command == "setoption") OnSetOptionCommand(stream);
    else if (command == "print") PrintBoard(pos);
    else if (command == "perft") OnPerftCommand(stream, pos);
    else if (command == "bench") OnBenchCommand(stream, pos);
    else if (command == "step") OnStepCommand(stream, pos);
    else if (command == "stop") OnStopCommand();
#ifdef USE_TUNING
    else if (command == "fit") {
        Tuner.Init(0);
        printf("info string current fit: %lf\n", Tuner.TexelFit(pos));
    } else if (command == "tune") {
        Params.TunePst();
    }
#endif
    else if (command == "quit") { return false; }
    return true;
}

void OnUciCommand() {

    std::cout << "id name " << engineName << " " << engineVersion << std::endl;
    std::cout << "id author " << engineAuthor << std::endl;
    std::cout << "option name Hash type spin default 16 min 1 max 4096" << std::endl;
    std::cout << "option name Clear Hash type button" << std::endl;
    std::cout << "option name NNUEfile type string default " << netPath << std::endl;
    std::cout << "option name nnueWeight type spin default "<<  nnueWeight << " min 0 max 200" << std::endl;
    std::cout << "option name hceWeight type spin default " << hceWeight << " min 0 max 200" << std::endl;
    std::cout << "uciok" << std::endl;
}

void OnPositionCommand(std::istringstream& stream, Position* pos) {

    std::string token, fen;
    stream >> token;

    // Set starting position
    if (token == "startpos") {
        fen = startFen;
        stream >> token;
    } 
    // Set kiwipete fen
    else if (token == "kiwipete") {
        fen = kiwipeteFen;
        stream >> token;
    }
    // Read fen
    else if (token == "fen") {
        while (stream >> token && token != "moves")
            fen += token + " ";
    }
    
    // Set position
    const char* charFen = fen.c_str();
    pos->Set(charFen);

    // Execute moves
    OnStepCommand(stream, pos);
}

void OnStepCommand(std::istringstream& stream, Position* pos) {

    std::string token;
    UndoData undo;

    for (bool found = true; stream >> token && found;) {
        pos->DoMove(StringToMove(pos, token), &undo);
        pos->TryMarkingIrreversible();
    }
}

void OnGoCommand(std::istringstream& stream, Position* pos) {

    std::string param, value;

    Timer.Clear();

    // Parse command
    stream >> param;
    while (!param.empty()) {
        if (param == "wtime") {
            stream >> value;
            Timer.SetData(wTime, std::stoi(value));
        }
        else if (param == "btime") {
            stream >> value;
            Timer.SetData(bTime, std::stoi(value));
        }
        else if (param == "winc") {
            stream >> value;
            Timer.SetData(wIncrement, std::stoi(value));
        }
        else if (param == "binc") {
            stream >> value;
            Timer.SetData(bIncrement, std::stoi(value));
        }
        else if (param == "movestogo") {
            stream >> value;
            Timer.SetRepeating(); // probable bugfix
            Timer.SetData(movesToGo, std::stoi(value));
        }
        else if (param == "ponder") {
            Timer.isPondering = true;
        }
        else if (param == "depth") {
            stream >> value;
            Timer.SetData(moveTime, INT_MAX);
            Timer.SetData(maxDepth, std::stoi(value));
        }
        else if (param == "movetime") {
            stream >> value;
            Timer.SetData(moveTime, std::stoi(value));
        }
        else if (param == "nodes") {
            stream >> value;
            Timer.SetData(moveTime, INT_MAX);
            Timer.SetData(maxNodes, std::stoi(value));
        } 
        else if (param == "infinite") {
            Timer.SetData(isInfinite, 1);
        }

        param.clear();
        stream >> param;
    }

    // UCI protocol recives basic time and increment 
    // separately for black and for white. Translate 
    // that to engine time / engine increment.
    Timer.SetDataForColor(pos->GetSideToMove());
    
    // How much time do we want to spend searching?
    Timer.SetMoveTiming();
    
    Move move; int unused;
    TT.Retrieve(pos->boardHash, &move, &unused, &unused, -Infinity, Infinity, 0, 0);
    Pv.Clear(move); // to reset ponder move and load possible best move from hash
    
    Think(pos);
    if (!Timer.IsInfiniteMode())
        Pv.SendBestMove();
}

void OnSetOptionCommand(std::istringstream& stream) {

    std::string token, name, value;

    stream >> token;

    while (stream >> token && token != "value")
        name += std::string(" ", !name.empty()) + token;

    while (stream >> token)
        value += std::string(" ", !value.empty()) + token;

    if (IsSameOrLowercase(name, "Hash")) {
        int val = std::stoi(value);
        TT.Allocate(val);
    }

    if (IsSameOrLowercase(name, "nnueWeight")) {
        nnueWeight = std::stoi(value);
    }

    if (IsSameOrLowercase(name, "hceWeight")) {
        hceWeight = std::stoi(value);
    }

    if (IsSameOrLowercase(name, "NNUEfile")) {
        
        TryLoadingNNUE(value.c_str());
    }
}

void OnBenchCommand(std::istringstream& stream, Position* pos) {

    int depth = 4; // default
    stream >> depth;
    std::cout << "Running perft test at depth " << depth << std::endl;
    Bench(pos, depth);
}

void OnPerftCommand(std::istringstream& stream, Position* pos) {

    int moveCount;
    int depth = 4; // default
    stream >> depth;
    std::cout << "Running perft test at depth " << depth << std::endl;

    Timer.Start();
    moveCount = Perft(pos, 0, depth, true);

    std::cout << "Perft " << depth 
              << " completed in " << Timer.Elapsed() 
              << " milliseconds, visiting " << moveCount 
              << " positions" << std::endl;
}

void OnNewGame(void) {

    History.Clear();
    TT.Clear();
}

void OnStopCommand() {

    Timer.isStopping = true;
    Timer.SetData(isInfinite, 0);

    // UCI protocol requires us to wait
    // after finishing "go infinite" run
    // due to reaching ply limit
    if (Timer.waitingForStop) {
        Timer.waitingForStop = false;
        Pv.SendBestMove();
    }
}

void TryLoadingNNUE(const char * path) {

    isNNUEloaded = NN.LoadFromFile(path);
    if (!isNNUEloaded)
        std::cout << "info string NNUE file " << path
        << " not found. Reverting to HCE eval." << std::endl;;
}
