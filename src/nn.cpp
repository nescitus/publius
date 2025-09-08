// Publius - Didactic public domain bitboard chess engine 
// by Pawel Koziol

// NNUE evaluation. Net architecture and constants make it
// equivalent to the simple example provided by the bullet trainer:
// https://github.com/jw1912/bullet/blob/main/examples/simple.rs
// The architecture is (768 -> HIDDEN_SIZE)x2 -> 1 and the only
// difference is that the number of hidden neurons is reduced to 32.

// This code draws some inspiration from Iris chess engine
// (https://github.com/citrus610/iris):
// I used it to check the math, separating NNUEparameters
// struct helped to create a nice file loader and the convention
// of using this-> helps to notice class members.

#include "types.h"
#include "piece.h"
#include "nn.h"

    // Constructor
    Net::Net() {
        this->Clear();
    }

    // Loads a net from the bullet-compatibile file
    bool Net::LoadFromFile(const char* path) {

        std::FILE* f = std::fopen(path, "rb");
        if (!f) return false;

        // Read the whole Parameters blob
        const size_t need = sizeof(PARAMS);
        const size_t got = std::fread(&PARAMS, 1, need, f);
        std::fclose(f);

        // size matches 
        return got == need;
    }

    // Returns NNUE evaluation of position
    i32 Net::GetScore(i8 color) {

        i32 score = 0;

        score += SumHalfAccumulator(this->accumulator[color], PARAMS.outputWeights[0]);
        score += SumHalfAccumulator(this->accumulator[!color], PARAMS.outputWeights[1]);

        return (score / L0_SCALE + PARAMS.outputBias) * EVAL_SCALE / (L0_SCALE * L1_SCALE);
    }

    // Adds "a feature" (a piece on a square) to the accumulator
    void Net::Add(i8 color, i8 type, i8 square) {

        // We need two indices, for white and black part
        // of the accumulator
        const auto indexWhite = Index(color, type, square);
        const auto indexBlack = Index(!color, type, square^56);

        // Update the accumulator
        for (int i = 0; i < HIDDEN_SIZE; ++i) {
            this->accumulator[0][i] += PARAMS.inputWeights[indexWhite][i];
            this->accumulator[1][i] += PARAMS.inputWeights[indexBlack][i];
        }
    }

    // Deletes "a feature" (a piece on a square) from the accumulator
    void Net::Del(i8 color, i8 type, i8 square) {

        const auto indexWhite = Index(color, type, square);
        const auto indexBlack = Index(!color, type, square^56);

        for (int i = 0; i < HIDDEN_SIZE; ++i) {
            this->accumulator[0][i] -= PARAMS.inputWeights[indexWhite][i];
            this->accumulator[1][i] -= PARAMS.inputWeights[indexBlack][i];
        }
    }

    // Clears the net (sets the empty board state)
    void Net::Clear() {

        for (i8 color = 0; color < 2; ++color) {
            for (int i = 0; i < HIDDEN_SIZE; ++i) {
                this->accumulator[color][i] = PARAMS.inputBiases[i];
            }
        }
    }

    // "Cold start" - setting values for a new board position
    void Net::Refresh(Position& pos) {

        this->Clear();

        for (i8 sq = 0; sq < 64; ++sq) {
            const i8 piece = pos.GetPiece((Square)sq);

            if (piece == noPiece)
                continue;

            const i8 type = (i8)TypeOfPiece((ColoredPiece)piece);
            const i8 color = (i8)ColorOfPiece((ColoredPiece)piece);

            this->Add(color, type, sq);
        }
    }