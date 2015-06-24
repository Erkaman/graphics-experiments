#pragma once

#include <string>
#include "common.hpp"

class Color {

public:
    float r,g,b,a;
public:

    Color():r(0.0f),g(0.0f),b(0.0f), a(0.0f) {  }

    Color(const float value): r(value), g(value),b(value), a(value){  }

    Color(const float r, const float g, const float b, const float a):r(r), g(g),b(b), a(a) { }

    Color(const float r, const float g, const float b):r(r), g(g),b(b), a(1.0f) { }


/*


    Vector3f(const Vector3f& that): m_x(that.m_x), m_y(that.m_y), m_z(that.m_z){ }

    friend Vector3f operator+(const Vector3f& v1, const Vector3f& v2);

    operator std::string() const {
	return "(" + std::to_string(m_x) + ", " + std::to_string(m_y) + ", " + std::to_string(m_z) + ")";
    }
*/
};
