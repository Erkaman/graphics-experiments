#include "vector2f.hpp"

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
