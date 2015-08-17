#pragma once

constexpr float PI = 3.14159265358f;

#include <math.h>
#include <algorithm>

inline bool FloatEquals(const float a, const float b) {
    constexpr static float EPSILON = 0.00001f;
    return fabs(a-b) < EPSILON;
}

inline float ToRadians(const float degrees) {
    return (degrees * PI) / 180.0f;
}

template <typename T>
T Clamp(const T& n, const T& lower, const T& upper) {
  return std::max(lower, std::min(n, upper));
}
