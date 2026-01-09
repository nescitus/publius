// Publius - Didactic public domain bitboard chess engine 
// by Pawel Koziol

// NNUE evaluation. Net architecture and constants make it
// equivalent to the simple example provided by the bullet trainer:
// https://github.com/jw1912/bullet/blob/main/examples/simple.rs
// The architecture is (768 -> HIDDEN_SIZE)x2 -> 1. Publius
// is able to read networks with any number of hidden neurons 
// from 16 to 256, as long as it is the multiple of 16.

// This code draws some inspiration from Iris chess engine
// (https://github.com/citrus610/iris):
// I used it to check the math, separating NNUEparameters
// struct helped to create a nice file loader and the convention
// of using this-> helps to notice class members.

#pragma once

#include <iostream>
#include <algorithm>
#include "position.h"

// int types

using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;

// Parameters are the same as in the simple example
// of the bullet trainer.

    // 64 squares * 5 piece types * two colors
    constexpr size_t INPUT_SIZE = 768;

    // Maximum size of a hidden layer. Must be
    // a multiple of 16. The engine is capable
    // of loading smaller nets, as long as the
    // above condition is satisfied.
    constexpr size_t HIDDEN_SIZE = 256;

    constexpr i32 EVAL_SCALE = 400;
    constexpr i32 L0_SCALE = 255;
    constexpr i32 L1_SCALE = 64;
    const i32 MUL_SCALE = L0_SCALE * L1_SCALE;

    // All the NNUE values in one struct
    // - that helps to read them from a file

    struct alignas(64) NNUEparameters
    {
        i16 inputWeights[INPUT_SIZE][HIDDEN_SIZE];
        i16 inputBiases[HIDDEN_SIZE];
        i16 outputWeights[2][HIDDEN_SIZE];
        i16 outputBias;
    };

    inline NNUEparameters PARAMS;

    // Actual NNUE class

    class Net
    {
    private:
        alignas(64) i16 accumulator[2][HIDDEN_SIZE];
        int networkWidth = HIDDEN_SIZE; // assume we are loading the biggest net available
        i32 SumHalfAccumulator(i16 inputs[HIDDEN_SIZE], i16 weights[HIDDEN_SIZE]);
    public:
        Net();
        i32 GetScore(i8 color);
        void Add(i8 color, i8 type, i8 square);
        void Del(i8 color, i8 type, i8 square);
        void Move(i8 color, i8 type, i8 addSq, i8 subSq);
        void Clear();
        void Refresh(Position& board);
        bool LoadFromFile(const char* path);
    };

    extern Net NN;

    // Calculating index to a neuron
    constexpr size_t Index(i8 color, i8 type, i8 square) {

        return size_t(384) * size_t(color) 
             + size_t(64) * size_t(type) 
             + size_t(square);
    }

    // Screlu reads as "squared clipped ReLu".
    // This activation function rejects negative
    // inputs, clips positive ones to a maximum
    // and returns power of two of a clipped input.
    constexpr i32 GetScrelu(i32 input) {

        i32 value = std::clamp(input, 0, L0_SCALE);
        return value * value;
    };