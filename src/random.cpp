#include "random.hpp"

#include "log.hpp"

#include <time.h>
#include <limits.h>

Random::Random(unsigned long long seed) {

    if(seed == 0) {
	LOG_E("The seed must be a non-zero value.");
    }

    m_seed = seed;
}

unsigned long long Random::Next() {
    m_seed ^= m_seed >> 12;
    m_seed ^= m_seed << 25;
    m_seed ^= m_seed >> 27;

    return m_seed * 2685821657736338717ull;
}

float Random::RandomFloat() {
    return (float) (Next()) / (float)(ULLONG_MAX);
}

float Random::RandomFloat(const float low, const float high) {
    return low + RandomFloat() * (high-low);
}
