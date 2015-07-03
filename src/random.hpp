#pragma once

// 64 xor-shift generator.
class Random {

private:

    unsigned long long m_seed;

public:

    Random(unsigned long long seed);
    Random();

    // random float in range [0,1]
    float RandomFloat();

    // random float in range [low,high]
    float RandomFloat(const float low, const float high);

    unsigned long long Next();

};
