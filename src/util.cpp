// Publius - Didactic public domain bitboard chess engine 
// by Pawel Koziol

#define NOMINMAX

#if defined(_WIN32) || defined(_WIN64)
#  include <windows.h>
#else
#  include <unistd.h>
#  include <sys/time.h>
#endif
#include <algorithm>
#include "types.h"
#include "square.h"
#include "position.h"
#include "util.h"

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

std::string SquareName(Square sq) {

    std::string result;
    result += FileOf(sq) + 'a';
    result += RankOf(sq) + '1';
    return result;
}