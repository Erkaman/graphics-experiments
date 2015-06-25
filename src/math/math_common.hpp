#pragma once

constexpr float PI = 3.14159265358f;

#include <math.h>

inline bool FloatEquals(const float a, const float b) {
    constexpr static float EPSILON = 0.00001f;
    return fabs(a-b) < EPSILON;
}
