// Publius - Didactic public domain bitboard chess engine 
// by Pawel Koziol

// NNUE evaluation. Net architecture and constants make it
// equivalent to the simple example provided by the bullet trainer:
// https://github.com/jw1912/bullet/blob/main/examples/simple.rs
// The architecture is (768 -> 128)x2 -> 1

// This code draws some inspiration from Iris chess engine
// (https://github.com/citrus610/iris):
// I used it to check the math, separating NNUEparameters
// struct helped to create a nice file loader and the convention
// of using this-> helps to notice class members.

// On top of that, there is an optional AVX2 fast path. It works
// the  same  as the simple addition of accumulator  values, but 
// processes 16 int16 lanes at a time using 256-bit vectors. 
//
// If AVX2 isn't available, we can still compile the scalar code 
// (#ifndef part), and performance is still correct - just slower.

//#define __AVX2__

#include "types.h"
#include "piece.h"
#include "nn.h"

#ifdef __AVX2__
#include <immintrin.h>
#endif

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

        return (score / L0_SCALE + PARAMS.outputBias) * EVAL_SCALE / MUL_SCALE;
    }

    // Adds "a feature" (a piece on a square) to the accumulator
    void Net::Add(i8 color, i8 type, i8 square) {

        // We need two indices, for white and black part
        // of the accumulator
        const auto indexWhite = Index(color, type, square);
        const auto indexBlack = Index(!color, type, square^56);

#if defined(__AVX2__)

        i16* __restrict a0 = &this->accumulator[0][0];
        i16* __restrict a1 = &this->accumulator[1][0];
        const i16* __restrict w0 = &PARAMS.inputWeights[indexWhite][0];
        const i16* __restrict w1 = &PARAMS.inputWeights[indexBlack][0];

        size_t i = 0;
        // 16 int16 lanes per __m256i
        for (; i + 16 <= HIDDEN_SIZE; i += 16) {
            __m256i A0 = _mm256_loadu_si256((const __m256i*)(a0 + i));
            __m256i W0 = _mm256_loadu_si256((const __m256i*)(w0 + i));
            __m256i A1 = _mm256_loadu_si256((const __m256i*)(a1 + i));
            __m256i W1 = _mm256_loadu_si256((const __m256i*)(w1 + i));

            A0 = _mm256_add_epi16(A0, W0);
            A1 = _mm256_add_epi16(A1, W1);

            _mm256_storeu_si256((__m256i*)(a0 + i), A0);
            _mm256_storeu_si256((__m256i*)(a1 + i), A1);
        }
#else
        // Update the accumulator
        for (size_t i = 0; i < HIDDEN_SIZE; ++i) {
            this->accumulator[0][i] += PARAMS.inputWeights[indexWhite][i];
            this->accumulator[1][i] += PARAMS.inputWeights[indexBlack][i];
        }
#endif
    }

    // Deletes "a feature" (a piece on a square) from the accumulator
    void Net::Del(i8 color, i8 type, i8 square) {

        const auto indexWhite = Index(color, type, square);
        const auto indexBlack = Index(!color, type, square^56);

#if defined(__AVX2__)

        i16* __restrict a0 = &this->accumulator[0][0];
        i16* __restrict a1 = &this->accumulator[1][0];
        const i16* __restrict w0 = &PARAMS.inputWeights[indexWhite][0];
        const i16* __restrict w1 = &PARAMS.inputWeights[indexBlack][0];

        size_t i = 0;
        for (; i + 16 <= HIDDEN_SIZE; i += 16) {
            __m256i A0 = _mm256_loadu_si256((const __m256i*)(a0 + i));
            __m256i W0 = _mm256_loadu_si256((const __m256i*)(w0 + i));
            __m256i A1 = _mm256_loadu_si256((const __m256i*)(a1 + i));
            __m256i W1 = _mm256_loadu_si256((const __m256i*)(w1 + i));

            A0 = _mm256_sub_epi16(A0, W0);
            A1 = _mm256_sub_epi16(A1, W1);

            _mm256_storeu_si256((__m256i*)(a0 + i), A0);
            _mm256_storeu_si256((__m256i*)(a1 + i), A1);
        }
#else

        for (size_t i = 0; i < HIDDEN_SIZE; ++i) {
            this->accumulator[0][i] -= PARAMS.inputWeights[indexWhite][i];
            this->accumulator[1][i] -= PARAMS.inputWeights[indexBlack][i];
        }

#endif
    }

    static inline void SetIndices(i8 color, i8 type, i8 sq, int& idxW, int& idxB) {
        idxW = Index(color, type, sq);
        idxB = Index(!color, type, sq ^ 56);
    }

    // a move operation performed on from and to squares at once
    // is slightly faster in AVX2 mode
    void Net::Move(i8 color, i8 type, i8 addSq, i8 subSq)
    {
        int addW, addB, subW, subB;
        SetIndices(color, type, addSq, addW, addB);
        SetIndices(color, type, subSq, subW, subB);

#if defined(__AVX2__)
        i16* __restrict a0 = &this->accumulator[0][0];
        i16* __restrict a1 = &this->accumulator[1][0];
        const i16* __restrict wAdd0 = &PARAMS.inputWeights[addW][0];
        const i16* __restrict wAdd1 = &PARAMS.inputWeights[addB][0];
        const i16* __restrict wSub0 = &PARAMS.inputWeights[subW][0];
        const i16* __restrict wSub1 = &PARAMS.inputWeights[subB][0];

        size_t i = 0;
        for (; i + 16 <= HIDDEN_SIZE; i += 16) {
            __m256i A0 = _mm256_loadu_si256((const __m256i*)(a0 + i));
            __m256i A1 = _mm256_loadu_si256((const __m256i*)(a1 + i));
            __m256i ADD0 = _mm256_loadu_si256((const __m256i*)(wAdd0 + i));
            __m256i ADD1 = _mm256_loadu_si256((const __m256i*)(wAdd1 + i));
            __m256i SUB0 = _mm256_loadu_si256((const __m256i*)(wSub0 + i));
            __m256i SUB1 = _mm256_loadu_si256((const __m256i*)(wSub1 + i));

            A0 = _mm256_add_epi16(A0, ADD0);
            A1 = _mm256_add_epi16(A1, ADD1);
            A0 = _mm256_sub_epi16(A0, SUB0);
            A1 = _mm256_sub_epi16(A1, SUB1);

            _mm256_storeu_si256((__m256i*)(a0 + i), A0);
            _mm256_storeu_si256((__m256i*)(a1 + i), A1);
        }

#else
        for (size_t i = 0; i < HIDDEN_SIZE; ++i) {
            this->accumulator[0][i] += PARAMS.inputWeights[addW][i];
            this->accumulator[1][i] += PARAMS.inputWeights[addB][i];
            this->accumulator[0][i] -= PARAMS.inputWeights[subW][i];
            this->accumulator[1][i] -= PARAMS.inputWeights[subB][i];
        }
#endif
    }

    // Clears the net (sets the empty board state)
    void Net::Clear() {

        for (i8 color = 0; color < 2; ++color) {
            for (size_t i = 0; i < HIDDEN_SIZE; ++i) {
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