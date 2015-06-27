#include "vector3f.hpp"

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
