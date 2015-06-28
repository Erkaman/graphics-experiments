#include "vector4f.hpp"
#include <math.h>
#include "math_common.hpp"

Vector4f::operator std::string() const {
    return "(" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) +  ", " + std::to_string(w) + ")";
}

float Vector4f::Length()const {
    return sqrt(x*x + y*y + z*z + w*w);
}

Vector4f& Vector4f::Normalize() {
    const float len = Length();

    x /= len;
    y /= len;
    z /= len;
    w /= len;

    return *this;
}

bool operator==(const Vector4f& v1, const Vector4f& v2) {
    constexpr static float EPSILON = 0.0001f;
    return fabs(v1.x - v2.x) <= EPSILON && fabs(v1.y - v2.y) <= EPSILON && fabs(v1.z - v2.z) <= EPSILON && fabs(v1.w - v2.w) <= EPSILON;

}

void Vector4f::Add(std::vector<float>& v)const {
    v.push_back(x);
    v.push_back(y);
    v.push_back(z);
    v.push_back(w);
}
