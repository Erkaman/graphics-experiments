#include "vector2f.hpp"

Vector2f::Vector2f(const float x_, const float y_): x(x_),y(y_){ }

Vector2f::Vector2f(): x(0.0f),y(0.0f){ }

Vector2f::operator std::string() const {

	return "(" + std::to_string(x) + ", " + std::to_string(y) + ")";
}
