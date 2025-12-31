// Publius - Didactic public domain bitboard chess engine 
// by Pawel Koziol

#pragma once

#include "move.h"

// Struct for saving search data related to a given depth
struct Stack {
	int captureSquare; // target square of previous capture if applicable, else -1
	int eval;          // eval for each ply, to see if we are improving or not
};

// SearchContext holds data generated and passed around during search
struct SearchContext {
	Stack stack[SearchTreeSize];
	Move excludedMove;
};

// Struct for remembering excluded moves in multiPV mode
struct ExcludedMoves {
    static constexpr int MAX_MOVES = 256;
    Move excluded[MAX_MOVES];
    int count = 0;

    void Clear() { count = 0; }

    void Add(Move m) {

        if (count < MAX_MOVES)
            excluded[count++] = m;
    }

    bool IsExcluded(Move m) const {
        
        for (int i = 0; i < count; ++i)
            if (excluded[i] == m)
                return true;

        return false;
    }
};

#include <vector>
#include <string>
#include <iostream>
#include <algorithm>

// Manages, saves and returns stuff needed for running multiPV search
struct MultiPVLines {

    struct Entry {
        int score = -Infinity;
        Move move = 0;          // root move for this PV
        std::string tail;       // e.g. " time ... nodes ... nps ... score ... pv ...\n"
    };

    std::vector<Entry> lines;
    int maxLines;

    explicit MultiPVLines(int maxLines_)
        : maxLines(maxLines_) {
        lines.reserve(maxLines);
    }

    void Clear() {
        lines.clear();
    }

    // Keep best->worst, cap at maxLines
    void Add(int score, Move move, const std::string& tail) {

        // Ignore empty PVs
        if (tail.empty() || move == 0) return;

        lines.push_back({ score, move, tail });
        std::stable_sort(lines.begin(), lines.end(),
            [](const Entry& a, const Entry& b) {
                return a.score > b.score;
            });

        if ((int)lines.size() > maxLines)
            lines.resize(maxLines);
    }

    // Returns best root move currently stored (0 if none)
    Move GetBestMove() const {
        return lines.empty() ? 0 : lines[0].move;
    }

    // Returns best score currently stored (-Infinity if none)
    int GetBestScore() const {
        return lines.empty() ? -Infinity : lines[0].score;
    }

    static std::string GetPlaceholderTail() {
        return " score cp 0 pv\n";
    }

    void DisplayAll(int depth, int wantedLines) const {

        const int k = std::min(wantedLines, maxLines);

        for (int i = 0; i < k; ++i) {
            std::cout << "info multipv " << (i + 1)
                << " depth " << depth
                << (i < (int)lines.size() ? lines[i].tail : GetPlaceholderTail());
        }
        std::cout << std::flush;
    }

    std::string GetTailOrPlaceholder(int idx) const {

        if (idx >= 0 && idx < (int)lines.size())
            return lines[idx].tail;
        return GetPlaceholderTail();
    }
};

extern ExcludedMoves rootExclusions;

void ClearSearchContext(SearchContext& context);
void Iterate(Position* pos, SearchContext* context);
int MultiPv(Position* pos, SearchContext* context, int depth);
int Widen(Position* pos, SearchContext* context, int depth, int lastScore);
int Search(Position* pos, SearchContext* context, int ply, int alpha, int beta, int depth, bool wasNullMove, bool isExcluded);
int Quiesce(Position* pos, int ply, int qdepth, int alpha, int beta);
bool SetImproving(const Stack &ppst, int eval, int ply);
void PrintRootInfo();
void TryInterrupting(void);