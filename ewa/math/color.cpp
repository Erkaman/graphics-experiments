#include "color.hpp"
#include <math.h>
Color::Color():r(0.0f),g(0.0f),b(0.0f), a(0.0f) {  }

Color::Color(const float value): r(value), g(value),b(value), a(value){  }

Color::Color(const float r, const float g, const float b, const float a):r(r), g(g),b(b), a(a) { }

Color::Color(const float r, const float g, const float b):r(r), g(g),b(b), a(1.0f) { }

Color Color::FromInt(const unsigned int r, const unsigned int g, const unsigned int b){
    return Color(
	(float)r / 255.0f,
	(float)g / 255.0f,
	(float)b / 255.0f,
	1.0f);
}

void Color::Add(std::vector<float>& v)const {
    v.push_back(r);
    v.push_back(g);
    v.push_back(b);
    v.push_back(a);

}

Color operator+(const Color& c1, const Color& c2) {
    return Color(
	c1.r+c2.r,
	c1.g+c2.g,
	c1.b+c2.b,
	c1.a+c2.a);
}

Color operator-(const Color& c1, const Color& c2) {
    return Color(
	c1.r-c2.r,
	c1.g-c2.g,
	c1.b-c2.b,
	c1.a-c2.a);
}


Color operator*(const float scale, const Color& c) {
    return Color(
	scale * c.r,
	scale * c.g,
	scale * c.b,
	scale * c.a);
}

Color operator*(const Color& c, const float scale) {
    return scale * c;
}


Color Color::Lerp(const Color& a, const Color& b, const float v) {
    return (b - a) * v + a;
}

bool operator==(const Color& c1, const Color& c2) {
    constexpr static float EPSILON = 0.0001f;
    return fabs(c1.r - c2.r) <= EPSILON && fabs(c1.g - c2.g) <= EPSILON && fabs(c1.b - c2.b) <= EPSILON && fabs(c1.a - c2.a) <= EPSILON;
}

Color::operator std::string() const {
    return "(" + std::to_string(r) + ", " + std::to_string(g) + ", " + std::to_string(b)  + ", " + std::to_string(a)+ ")";
}

Color Color::White() {
    return Color(1.0,1.0,1.0);
}
