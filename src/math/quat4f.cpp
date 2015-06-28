#include "quat4f.hpp"
#include <math.h>

Quat4f::Quat4f(float x_, float y_, float z_, float w_): x(x_), y(y_),z(z_),w(w_){
    Normalize();
}

Quat4f& Quat4f::Normalize(){
    float norm = (x*x + y*y + z*z + w*w);

    if (norm > 0.0f) {
	norm = 1.0f/(float)sqrt(norm);
	x = norm*x;
	y = norm*y;
	z = norm*z;
	w = norm*w;
    } else {
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;
	w = 0.0f;
    }
    return (*this);
}


Quat4f& Quat4f::operator *=(const Quat4f& that){
    *this = (*this) * that;
    return (*this);
}


Quat4f& Quat4f::Conjugate(){
    x = -x;
    y = -y;
    z = -z;

    return *this;
}


Quat4f::operator std::string() const {
    return "(" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z)  + ", " + std::to_string(w)+ ")";
}

Quat4f operator *(const Quat4f& q1, const Quat4f& q2){

    return Quat4f(
	q1.w*q2.x + q2.w*q1.x + q1.y*q2.z - q1.z*q2.y,
	q1.w*q2.y + q2.w*q1.y - q1.x*q2.z + q1.z*q2.x,
	q1.w*q2.z + q2.w*q1.z + q1.x*q2.y - q1.y*q2.x,
	q1.w*q2.w - q1.x*q2.x - q1.y*q2.y - q1.z*q2.z
	);
}
