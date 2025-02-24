#define NOMINMAX

#include <random>
#include <cmath>
#include <string.h>
#if defined(_WIN32) || defined(_WIN64)
#  include <windows.h>
#else
#  include <unistd.h>
#  include <sys/time.h>
#endif
#include "publius.h"

std::mt19937_64 e2(2018);
std::uniform_int_distribution<Bitboard> dist(std::llround(std::pow(2, 56)), std::llround(std::pow(2, 62)));

Bitboard Random64(void) {
    Bitboard result = dist(e2);
    return result;
}

// TODO: model after hakkapeliitta

int InputAvailable(void) {

#if defined(_WIN32) || defined(_WIN64)
    static int init = 0, pipe;
    static HANDLE inh;
    DWORD dw;

    if (!init) {
        init = 1;
        inh = GetStdHandle(STD_INPUT_HANDLE);
        pipe = !GetConsoleMode(inh, &dw);
        if (!pipe) {
            SetConsoleMode(inh, dw & ~(ENABLE_MOUSE_INPUT | ENABLE_WINDOW_INPUT));
            FlushConsoleInputBuffer(inh);
        }
    }
    if (pipe) {
        if (!PeekNamedPipe(inh, NULL, 0, NULL, &dw, NULL))
            return 1;
        return dw > 0;
    }
    else {
        GetNumberOfConsoleInputEvents(inh, &dw);
        return dw > 1;
    }
#else
    fd_set readfds;
    struct timeval tv;

    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    select(STDIN_FILENO + 1, &readfds, NULL, NULL, &tv);
    return FD_ISSET(STDIN_FILENO, &readfds);
#endif
}

int StringToMove(Position *pos, const std::string& moveString) {

    Square from, to;
    int type;
    std::string move_str;
    move_str = moveString;

    from = MakeSquare(move_str[0] - 'a', move_str[1] - '1');
    to = MakeSquare(move_str[2] - 'a', move_str[3] - '1');
    type = tNormal;

    if (pos->PieceTypeOnSq(from) == King && std::abs(to - from) == 2) {
        type = tCastle;
    }
    else if (pos->PieceTypeOnSq(from) == Pawn) {
        if (to == pos->EnPassantSq()) {
            type = tEnPassant;
        }
        else if (std::abs(to - from) == 16) {
            type = tPawnjump;
        }
        else if (move_str.length() > 4 && move_str[4] != '\0') {
            switch (move_str[4]) {
            case 'n':
                type = tPromN;
                break;
            case 'b':
                type = tPromB;
                break;
            case 'r':
                type = tPromR;
                break;
            case 'q':
                type = tPromQ;
                break;
            }
        }
    }

    return CreateMove(from, to, type);
}

void RefreshPv(int ply, int move) {

    pvLine[ply][ply] = move;

    for (int j = ply + 1; j < pvSize[ply + 1]; ++j)
        pvLine[ply][j] = pvLine[ply + 1][j];

    pvSize[ply] = pvSize[ply + 1];
}

int Clip(int v, int l) {
    return (std::max(-l, std::min(v, l)));
}

// Function to convert a string to lowercase
std::string ToLower(const std::string& str) {
    std::string lowerStr = str;
    std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);
    return lowerStr;
}

// Function to compare two strings
bool IsSameOrLowercase(const std::string& str1, const std::string& str2) {
    return str1 == str2 || ToLower(str1) == ToLower(str2);
}
