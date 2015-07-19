#include "vector3f.hpp"
#include "math/quat4f.hpp"
#include <math.h>
#include "math_common.hpp"

Vector3f& Vector3f::Rotate(const float angle, const Vector3f& axis) {

    // see page 75, equation 4.40 in real-time rendering for an explanation of the below code.

    float sinHalfAngle = (float) sin(toRadians(angle / 2.0f));
    float cosHalfAngle = (float) cos(toRadians(angle / 2.0f));

    float rX = axis.x * sinHalfAngle;
    float rY = axis.y * sinHalfAngle;
    float rZ = axis.z * sinHalfAngle;
    float rW = cosHalfAngle;

    Quat4f q(rX, rY, rZ, rW);
    Quat4f q_conj(q); q_conj.Conjugate();
    Quat4f p(this->x, this->y ,this->z, 0.0f);

    Quat4f result = q * p * q_conj;

    this->x = result.x;
    this->y = result.y;
    this->z = result.z;

    return (*this);
}


Vector3f::operator std::string() const {
    return "(" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ")";
}


float Vector3f::Length()const {
    return sqrt(x*x + y*y + z*z);
}

Vector3f& Vector3f::Normalize() {
    const float len = Length();

    x /= len;
    y /= len;
    z /= len;

    return *this;
}

Vector3f Vector3f::Normalize()const {
    Vector3f n(*this);
    n.Normalize();
    return n;
}


Vector3f Vector3f::Cross(const Vector3f& v1, const Vector3f& v2) {
    return Vector3f(
	v1.y * v2.z - v1.z * v2.y,
	v1.z * v2.x - v1.x * v2.z,
	v1.x * v2.y - v1.y * v2.x
	);
}

bool operator==(const Vector3f& v1, const Vector3f& v2) {
    constexpr static float EPSILON = 0.0001f;
    return fabs(v1.x - v2.x) <= EPSILON && fabs(v1.y - v2.y) <= EPSILON && fabs(v1.z - v2.z) <= EPSILON;
}


void Vector3f::Add(std::vector<float>& v)const {
    v.push_back(x);
    v.push_back(y);
    v.push_back(z);
}
