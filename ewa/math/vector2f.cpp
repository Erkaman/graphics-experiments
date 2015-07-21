#include "vector2f.hpp"

#include <math.h>


Vector2f::Vector2f(const float x_, const float y_): x(x_),y(y_){ }


Vector2f::Vector2f(const float x_): Vector2f(x_,x_) {}


Vector2f::Vector2f(): x(0.0f),y(0.0f){ }

Vector2f::operator std::string() const {

	return "(" + std::to_string(x) + ", " + std::to_string(y) + ")";
}


void Vector2f::Add(std::vector<float>& v)const {
    v.push_back(x);
    v.push_back(y);
}


float Vector2f::Length()const {
    return sqrt(x*x + y*y);
}

Vector2f& Vector2f::Normalize() {
    const float len = Length();

    x /= len;
    y /= len;

    return *this;
}

Vector2f Vector2f::Normalize()const {
    Vector2f n(*this);
    n.Normalize();
    return n;
}

bool operator==(const Vector2f& v1, const Vector2f& v2) {
    constexpr static float EPSILON = 0.0001f;
    return fabs(v1.x - v2.x) <= EPSILON && fabs(v1.y - v2.y) <= EPSILON;
}
