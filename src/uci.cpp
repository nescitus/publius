#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include "color.h"
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

    std::string cmd;
    stream >> cmd;

    if (cmd == "isready") std::cout << "readyok" << std::endl;
    else if (cmd == "ucinewgame") OnNewGame();
    else if (cmd == "uci") OnUciCommand();
    else if (cmd == "position") OnPositionCommand(stream, pos);
    else if (cmd == "go") OnGoCommand(stream, pos);
    else if (cmd == "setoption") OnSetOptionCommand(stream);
    else if (cmd == "print") PrintBoard(pos);
    else if (cmd == "perft") OnPerftCommand(stream, pos);
    else if (cmd == "step") OnStepCommand(stream, pos);
    else if (cmd == "quit") { return false; }
    return true;
}

void OnUciCommand() {

    std::cout << "id name Publius 0.036" << std::endl;
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

    std::string param, read, moveStr, ponderStr;

    Timer.Clear();
    State.isPondering = false;

    // Parse command
    stream >> param;
    while (!param.empty()) {
        if (param == "wtime") {
            stream >> read;
            Timer.SetData(wTime, std::stoi(read));
        }
        else if (param == "btime") {
            stream >> read;
            Timer.SetData(bTime, std::stoi(read));
        }
        else if (param == "winc") {
            stream >> read;
            Timer.SetData(wIncrement, std::stoi(read));
        }
        else if (param == "binc") {
            stream >> read;
            Timer.SetData(bIncrement, std::stoi(read));
        }
        else if (param == "movestogo") {
            stream >> read;
            Timer.SetData(movesToGo, std::stoi(read));
        }
        else if (param == "ponder") {
            State.isPondering = true;
        }
        else if (param == "depth") {
            stream >> read;
            Timer.SetData(isInfinite, 1);
            Timer.SetData(maxDepth, std::stoi(read));
        }
        else if (param == "movetime") {
            stream >> read;
            Timer.SetData(moveTime, std::stoi(read));
        }
        else if (param == "nodes") {
            stream >> read;
            Timer.SetData(maxNodes, std::stoi(read));
        }

        param.clear();
        stream >> param;
    }

    Timer.SetSideData(pos->GetSideToMove());
    Timer.SetMoveTiming();

    Pv.line[0][0] = 0; // clear engine move
    Pv.line[0][1] = 0; // clear ponder move
    Think(pos);

    if (Pv.line[0][1]) {
        std::cout << "bestmove " << MoveToString(Pv.line[0][0])
                  << " ponder "  << MoveToString(Pv.line[0][1]) 
                  << std::endl;
    }
    else
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

void OnPerftCommand(std::istringstream& stream, Position* p) {

    int depth, moveCount;

    depth = 4;
    stream >> depth;
    std::cout << "Running perft test at depth " << depth << std::endl;

    Timer.SetStartTime();
    moveCount = Perft(p, 0, depth, true);

    std::cout << "Perft " << depth 
              << " completed in " << Timer.Elapsed() 
              << " milliseconds, visiting " << moveCount 
              << " positions" << std::endl;
}

void OnNewGame(void) {

    History.Clear();
    TT.Clear();
}