#pragma once

#include <string>
#include "common.hpp"
#include "math_common.hpp"

class Vector3f {

public:

    float x,y,z;

public:

    Vector3f():x(0.0f),y(0.0f),z(0.0f) {  }

    Vector3f(const float value_): x(value_), y(value_),z(value_){  }

    Vector3f(const float x_, const float y_, const float z_): x(x_),y(y_),z(z_){ }

    Vector3f(const Vector3f& that): x(that.x), y(that.y), z(that.z){ }

    friend Vector3f operator+(const Vector3f& v1, const Vector3f& v2);
    friend Vector3f operator-(const Vector3f& v1, const Vector3f& v2);
    friend bool operator==(const Vector3f& v1, const Vector3f& v2);
    friend bool operator!=(const Vector3f& v1, const Vector3f& v2);

    Vector3f operator-() const {
	Vector3f v(-x,-y,-z);
	return v;
    }

    operator std::string() const {
	return "(" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ")";
    }

    float Length() {
	return sqrt(x*x + y*y + z*z);
    }

    void Normalize() {
	const float len = Length();

	x /= len;
	y /= len;
	z /= len;
    }

    static Vector3f Cross(const Vector3f& v1, const Vector3f& v2) {
	return Vector3f(
	    v1.y * v2.z - v1.z * v2.y,
	    v1.z * v2.x - v1.x * v2.z,
	    v1.x * v2.y - v1.y * v2.x
	    );
    }

};

inline Vector3f operator+(const Vector3f& v1, const Vector3f& v2) {
    return Vector3f(
	v1.x+v2.x,
	v1.y+v2.y,
	v1.z+v2.z);
}

inline Vector3f operator-(const Vector3f& v1, const Vector3f& v2) {
    return v1 + (-v2);
}

inline bool operator==(const Vector3f& v1, const Vector3f& v2) {
    constexpr static float EPSILON = 0.0001f;
    return fabs(v1.x - v2.x) <= EPSILON && fabs(v1.y - v2.y) <= EPSILON && fabs(v1.z - v2.z) <= EPSILON;
}

inline bool operator!=(const Vector3f& v1, const Vector3f& v2) {
    return !(v1 == v2);
}
