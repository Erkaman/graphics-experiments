#include "vector2i.hpp"

#include <math.h>

Vector2i::operator std::string() const {
    return "(" + std::to_string(x) + ", " + std::to_string(y) + ")";
}

float Vector2i::Length()const {
    return sqrt(x*x + y*y);
}

bool operator==(const Vector2i& v1, const Vector2i& v2) {
    return v1.x == v2.x && v1.y == v2.y;
}
