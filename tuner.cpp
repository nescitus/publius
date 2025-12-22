// Publius - Didactic public domain bitboard chess engine by Pawel Koziol

#include <cmath>
#include <iostream> // for cout
#include "types.h"
#include "square.h"
#include "position.h"
#include "publius.h" // for USE_TUNING
#include "move.h"
#include "timer.h"
#include "score.h"
#include "evaldata.h"
#include "score.h"
#include "params.h"
#include "api.h"
#include "util.h" // for SquareName
#include "tuner.h"

#ifdef USE_TUNING

void cTuner::Init(int filter) {
    dataset.clear();
    dataset.reserve(7'200'000); // guess to reduce reallocations; safe if exceeded

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
        if (line.find("1/2-1/2") != std::string::npos) res = 0.5;
        else if (line.find("1-0") != std::string::npos) res = 1.0;
        else if (line.find("0-1") != std::string::npos) res = 0.0;
        else continue; // skip lines without a result tag

        dataset.push_back({ std::move(line), res });
    }

    std::cout << readCnt << " total lines read, " << dataset.size() << " usable samples\n";
}

double cTuner::TexelFit(Position* p) {

    if (dataset.empty()) return 0.0;

    EvalData e;
    double sum = 0.0;

    const double k_const = 1.325;
    const double a = k_const * std::log(10.0) / 400.0; // precompute

    int iteration = 0;
    for (const auto& s : dataset) {
        ++iteration;

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

double cTuner::TuneSingleSquare(Position* pos, Parameters* params,
       int piece, Square s, int delta, double currentFit) {

    currentFit = TryChangeMgPst(pos, params, piece, s, delta, currentFit);
    currentFit = TryChangeMgPst(pos, params, piece, s, -delta, currentFit);
    currentFit = TryChangeMgPst(pos, params, piece, s, delta, currentFit);
    currentFit = TryChangeMgPst(pos, params, piece, s, -delta, currentFit);

    // Due to symmetry, endgame tables are updated twice;
    // we compensate for that by calling TryChangeMgPst()
    // more often.
    currentFit = TryChangeEgPst(pos, params, piece, s, delta, currentFit);
    currentFit = TryChangeEgPst(pos, params, piece, s, -delta, currentFit);
    return currentFit;
}

double cTuner::TryChangeMgPst(Position* pos, Parameters* params,
       int piece, Square sq, int delta, double baselineLoss) {

    int oldWeight = Params.pst[White][piece][sq];
    int mgWeight = ScoreMG(oldWeight);
    int egWeight = ScoreEG(oldWeight);
    int mirror = MirrorRank(sq);

    params->pst[White][piece][sq] = MakeScore(mgWeight + delta, egWeight);
    params->pst[Black][piece][mirror] = MakeScore(mgWeight + delta, egWeight);

    double newLoss = Tuner.TexelFit(pos);

    if (newLoss + 1e-12 < baselineLoss) {
        SuccessMessage("MG success: piece ", piece, sq, delta, newLoss, baselineLoss);
        return newLoss; // keep new values
    } else {
        params->pst[White][piece][sq] = oldWeight;
        params->pst[Black][piece][mirror] = oldWeight;
        std::cout << "MG no-improve: loss " << baselineLoss << "\n";
        return baselineLoss; // revert
    }
}

double cTuner::TryChangeEgPst(Position* pos, Parameters* params,
       int piece, Square sq, int delta, double baselineLoss) {

    // File-mirror inside same color
    const Square floppedSquare = MirrorFile(sq);

    // Map to black by rank-mirroring
    const Square blackSquare = MirrorRank(sq);
    const Square blackFlopped = MirrorRank(floppedSquare);

    // Save old packed score (we need separate weights
    // even though endgame tables are symmetric,
    // because midgame tables aren't, and so isn't
    // the packed score.
    const int wOld1 = params->pst[White][piece][sq];
    const int wOld2 = params->pst[White][piece][floppedSquare];

    auto bumpEG = [&](int packed, int d) {
        return MakeScore(ScoreMG(packed), ScoreEG(packed) + d);
    };

    // Apply symmetric EG adjustments
    params->pst[White][piece][sq] = bumpEG(wOld1, delta);
    params->pst[White][piece][floppedSquare] = bumpEG(wOld2, delta);
    params->pst[Black][piece][blackSquare] = bumpEG(wOld1, delta);
    params->pst[Black][piece][blackFlopped] = bumpEG(wOld2, delta);

    const double newLoss = Tuner.TexelFit(pos);

    if (newLoss + 1e-12 < baselineLoss) {
        SuccessMessage("EG success: piece ", piece, sq, delta, newLoss, baselineLoss);
        return newLoss; // keep
    }

    // Revert all four squares
    params->pst[White][piece][sq] = wOld1;
    params->pst[White][piece][floppedSquare] = wOld2;
    params->pst[Black][piece][blackSquare] = wOld1;
    params->pst[Black][piece][blackFlopped] = wOld2;

    std::cout << "EG no-improve: loss " << baselineLoss << "\n";
    return baselineLoss;
}

void cTuner::SuccessMessage(std::string intro, int piece, Square sq, int delta, double newLoss, double baselineLoss) {

    std::cout << intro << piece << " sq " << SquareName(sq)
              << " delta " << delta << " -> loss " << newLoss
              << " (was " << baselineLoss << ")\n";
}

#endif
