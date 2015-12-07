#pragma once

#include <string>
#include <vector>
#include "vector3f.hpp"

class Vector4f {

public:

/*
  INSTANCE VARIABLES
 */

    float x,y,z,w;

public:

    /*
      CONSTRUCTORS
     */

    Vector4f():x(0.0f),y(0.0f),z(0.0f),w(0.0f) {  }

    Vector4f(const float value_): x(value_), y(value_),z(value_),w(value_){  }

    Vector4f(const float x_, const float y_, const float z_, const float w_): x(x_),y(y_),z(z_),w(w_){ }

    Vector4f(const Vector4f& that): x(that.x), y(that.y), z(that.z),w(that.w){ }

    Vector4f(const Vector3f& that, const float w_): x(that.x), y(that.y), z(that.z),w(w_){ }

    /*
      OPERATOR OVERLOADS
     */

    friend Vector4f operator+(const Vector4f& v1, const Vector4f& v2);
    friend Vector4f operator-(const Vector4f& v1, const Vector4f& v2);
    friend bool operator==(const Vector4f& v1, const Vector4f& v2);
    friend bool operator!=(const Vector4f& v1, const Vector4f& v2);
    friend Vector4f operator*(const float scale, const Vector4f& v);
    Vector4f& operator += (const Vector4f& that);
    Vector4f operator-() const;
    operator std::string() const;
    operator Vector3f() const;

    /*
      PUBLIC METHODS
     */


    float Length()const;

    Vector4f& Normalize();

    /*
      Rotate this vector around the axis.
     */
    Vector4f& Rotate(const float angle, const Vector4f& axis);

    /*
      Add this vector to an array of floats.
     */
    void Add(std::vector<float>& v) const;

};


/*
  OPERATOR DEFINITIONS
 */

inline Vector4f operator+(const Vector4f& v1, const Vector4f& v2) {
    return Vector4f(
	v1.x+v2.x,
	v1.y+v2.y,
	v1.z+v2.z,
	v1.w+v2.w);
}

inline Vector4f operator-(const Vector4f& v1, const Vector4f& v2) {
    return v1 + (-v2);
}

inline bool operator!=(const Vector4f& v1, const Vector4f& v2) {
    return !(v1 == v2);
}


inline Vector4f operator*(const float scale, const Vector4f& v) {
    return Vector4f(
	scale * v.x,
	scale * v.y,
	scale * v.z,
	scale * v.w);
}

inline Vector4f& Vector4f::operator += (const Vector4f& that){
    *this = (*this) + that;
    return (*this);
}

inline Vector4f Vector4f::operator-() const {
    return Vector4f(-x,-y,-z,-w);
}
