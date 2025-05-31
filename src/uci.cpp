#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include "types.h"
#include "square.h"
#include "limits.h"
#include "publius.h"
#include "timer.h"
#include "history.h"
#include "trans.h"
#include "move.h"
#include "search.h"
#include "uci.h"
#include "evaldata.h"
#include "eval.h"
#include "pv.h"

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
    else if (command == "quit") { return false; }
    return true;
}

void OnUciCommand() {

    std::cout << "id name Publius 0.059" << std::endl;
    std::cout << "id author Pawel Koziol" << std::endl;
    std::cout << "option name Hash type spin default 16 min 1 max 4096" << std::endl;
    std::cout << "option name Clear Hash type button" << std::endl;
    std::cout << "uciok" << std::endl;
}

void OnPositionCommand(std::istringstream& stream, Position* pos) {

    std::string token, fen;
    stream >> token;

    if (token == "startpos") {
        fen = startFen;
        stream >> token;
    } 
    else if (token == "kiwipete") {
        fen = kiwipeteFen;
        stream >> token;
    }
    else if (token == "fen") {
        while (stream >> token && token != "moves")
            fen += token + " ";
    }
    const char* charFen = fen.c_str();

    pos->Set(charFen);

    for (bool found = true; stream >> token && found;)
    {
        pos->DoMove(StringToMove(pos, token), 0);
        pos->TryMarkingIrreversible();
    }
}

void OnStepCommand(std::istringstream& stream, Position* pos) {

    std::string token;

    for (bool found = true; stream >> token && found;)
    {
        char* moveChar = const_cast<char*>(token.c_str());
        pos->DoMove(StringToMove(pos, moveChar), 0);
        pos->TryMarkingIrreversible();
    }
}

void OnGoCommand(std::istringstream& stream, Position* pos) {

    std::string param, value;

    Timer.Clear();
    State.isPondering = false;

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
            Timer.isRepeating = true; // probable bugfix
            Timer.SetData(movesToGo, std::stoi(value));
        }
        else if (param == "ponder") {
            State.isPondering = true;
        }
        else if (param == "depth") {
            stream >> value;
            Timer.SetData(isInfinite, 1);
            Timer.SetData(maxDepth, std::stoi(value));
        }
        else if (param == "movetime") {
            stream >> value;
            Timer.SetData(moveTime, std::stoi(value));
        }
        else if (param == "nodes") {
            stream >> value;
            Timer.SetData(isInfinite, 1);
            Timer.SetData(maxNodes, std::stoi(value));
        }

        param.clear();
        stream >> param;
    }

    // UCI protocol differentiates between
    // basic time and increment for black
    // and for white. Translate that to
    // engine time / engine increment
    Timer.SetDataForColor(pos->GetSideToMove());
    
    // How much time do we want to spend searching?
    Timer.SetMoveTiming();

    Pv.line[0][0] = 0; // clear engine move
    Pv.line[0][1] = 0; // clear ponder move
    Think(pos);

    if (Pv.line[0][1]) {
        // print best move and ponder move
        std::cout << "bestmove " << MoveToString(Pv.line[0][0])
                  << " ponder "  << MoveToString(Pv.line[0][1]) 
                  << std::endl;
    }
    else
        // print just best move
        std::cout << "bestmove " << MoveToString(Pv.line[0][0]) 
                  << std::endl;
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

    Timer.SetStartTime();
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