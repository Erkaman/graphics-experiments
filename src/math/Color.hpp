#pragma once

#include <string>
#include "common.hpp"

class Color {

private:

    float m_r,m_g,m_b,m_a;

public:

    Color():m_r(0.0f),m_g(0.0f),m_b(0.0f), m_a(0.0f) {  }

    Color(const float value): m_r(value), m_g(value),m_b(value), m_a(value){  }

    Color(const float r, const float g, const float b, const float a):m_r(r), m_g(g),m_b(b), m_a(a) { }

    Color(const float r, const float g, const float b):m_r(r), m_g(g),m_b(b), m_a(1.0f) { }


    float GetRed()const {
	return m_r;
    }

    float GetGreen()const {
	return m_g;
    }

    float GetBlue()const {
	return m_b;
    }

    float GetAlpha()const {
	return m_a;
    }

/*


    Vector3f(const Vector3f& that): m_x(that.m_x), m_y(that.m_y), m_z(that.m_z){ }

    friend Vector3f operator+(const Vector3f& v1, const Vector3f& v2);

    operator std::string() const {
	return "(" + std::to_string(m_x) + ", " + std::to_string(m_y) + ", " + std::to_string(m_z) + ")";
    }
*/
};
