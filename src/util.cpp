// Publius - Didactic public domain bitboard chess engine by Pawel Koziol

#define NOMINMAX

#if defined(_WIN32) || defined(_WIN64)
#  include <windows.h>
#else
#  include <unistd.h>
#  include <sys/time.h>
#endif
#include "types.h"
#include "position.h"
#include "publius.h"
#include "move.h"
#include "timer.h"
#include "score.h"
#include "evaldata.h"
#include "eval.h" // tuner

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

#ifdef USE_TUNING

void cTuner::Init(int filter) {
    dataset.clear();
    dataset.reserve(5'800'000); // guess to reduce reallocations; safe if exceeded

    std::ifstream in("c:/test/epd/lichess_resolved.epd");
    std::cout << "reading quiet.epd: " << (in ? "success" : "failure") << "\n";
    if (!in) return;

    std::srand(Timer.Now()); // keep your RNG source

    std::string line;
    int readCnt = 0;

    while (std::getline(in, line)) {
        if (filter) {
            if ((std::rand() % 100000) > filter) continue;
        }

        ++readCnt;
        if ((readCnt % 1'000'000) == 0)
            std::cout << readCnt << " positions loaded\n";

        // detect result; most EPD sets put it at end
        double res;
        if (line.find("1/2-1/2") != std::string::npos) res = 0.5; // fallback
        else if (line.find("1-0") != std::string::npos) res = 1.0;
        else if (line.find("0-1") != std::string::npos) res = 0.0;
        else if (line.find("[0.5]") != std::string::npos) res = 0.5; // fallback
        else if (line.find("[1.0]") != std::string::npos) res = 1.0;
        else if (line.find("[0.0]") != std::string::npos) res = 0.0;
        else continue; // skip lines without a result tag

        dataset.push_back({ std::move(line), res });
    }

    std::cout << readCnt << " total lines read, " << dataset.size() << " usable samples\n";
}

double cTuner::TexelFit(Position* p) {

    if (dataset.empty()) return 0.0;

    EvalData e;
    double sum = 0.0;

    // 1 / (1 + 10^(-k*score/400))  ==  1 / (1 + exp(-(k*ln10/400)*score))
    const double k_const = 1.325; // 77.263200
    const double a = k_const * std::log(10.0) / 400.0; // precompute

    int iteration = 0;
    for (const auto& s : dataset) {
        ++iteration;

        // assumes you have Position::Set(const std::string&). If not, add it.
        p->Set(s.epd);

        int score = Evaluate(p, &e);
        if (p->GetSideToMove() == Black)
            score = -score;

        const double sigmoid = 1.0 / (1.0 + std::exp(-a * (double)score));
        const double diff = (s.result - sigmoid);
        sum += diff * diff;
    }

    return 1000.0 * (sum / iteration);
}

#endif