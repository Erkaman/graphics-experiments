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


template <class T> const T& my_max(const T& a, const T& b) {
	return (a<b) ? b : a;     // or: return comp(a,b)?b:a; for version (2)
}

template <class T> const T& my_min(const T& a, const T& b) {
	return !(b<a) ? a : b;     // or: return !comp(b,a)?a:b; for version (2)
}

template <typename T>
T Clamp(const T& n, const T& lower, const T& upper) {
  return my_max(lower, my_min(n, upper));
}
