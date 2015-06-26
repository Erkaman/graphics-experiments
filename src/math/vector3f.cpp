#include "vector3f.hpp"

Vector3f& Vector3f::Rotate(const float angle, const Vector3f& axis) {

    float sinHalfAngle = (float) sin(toRadians(angle / 2));
    float cosHalfAngle = (float) cos(toRadians(angle / 2));

    float rX = axis.x * sinHalfAngle;
    float rY = axis.y * sinHalfAngle;
    float rZ = axis.z * sinHalfAngle;
    float rW = cosHalfAngle;

    Quat4f result(rX, rY, rZ, rW);
    Quat4f rotation(rX, rY, rZ, rW); rotation.Conjugate();
    Quat4f p(this->x, this->y ,this->z, 0.0f);

    result *= p;

    result *= rotation;

    this->x = result.x;
    this->y = result.y;
    this->z = result.z;

    return (*this);
}
