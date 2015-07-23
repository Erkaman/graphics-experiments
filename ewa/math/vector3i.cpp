#include "vector3i.hpp"

#include <math.h>

Vector3i::operator std::string() const {
    return "(" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ")";
}

float Vector3i::Length()const {
    return sqrt(x*x + y*y + z*z);
}

Vector3i Vector3i::Cross(const Vector3i& v1, const Vector3i& v2) {
    return Vector3i(
	v1.y * v2.z - v1.z * v2.y,
	v1.z * v2.x - v1.x * v2.z,
	v1.x * v2.y - v1.y * v2.x
	);
}

bool operator==(const Vector3i& v1, const Vector3i& v2) {
    return v1.x == v2.x && v1.y == v2.y && v1.z == v2.z;
}
