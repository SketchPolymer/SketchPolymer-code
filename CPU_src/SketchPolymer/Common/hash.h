#ifndef HASH_H
#define HASH_H

#include <random>
#include <iostream>

#include <limits.h>
#include <stdint.h>
#include "BOBHash.h"
#include "murmur3.h"

template<typename T>
inline uint32_t hash(const T& data, uint32_t seed = 0);
inline uint32_t randomGenerator();

static std::random_device rd;
static std::mt19937 rng(rd());
static std::uniform_real_distribution<double> dis(0, 1);

inline uint32_t randomGenerator(){
    return rng();
}

template<typename T>
inline uint32_t hash(const T& data, uint32_t seed){
    return BOBHash::BOBHash32((uint8_t*)&data, sizeof(T), seed);
    // uint32_t output;
    // MurmurHash3_x86_32(&data, sizeof(T), seed, &output);
    // return output;
}

#endif
