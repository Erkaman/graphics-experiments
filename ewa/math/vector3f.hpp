#pragma once

#include <string>
#include <vector>

class Vector4f;

class Vector3f {

public:

/*
  INSTANCE VARIABLES
 */

    float x,y,z;


public:

    /*
      CONSTRUCTORS
     */

    Vector3f():x(0.0f),y(0.0f),z(0.0f) {  }

    Vector3f(const float value_): x(value_), y(value_),z(value_){  }

    Vector3f(const float x_, const float y_, const float z_): x(x_),y(y_),z(z_){ }

    Vector3f(const Vector3f& that): x(that.x), y(that.y), z(that.z){ }
    Vector3f(const Vector4f& that);

    /*
      OPERATOR OVERLOADS
     */

    friend Vector3f operator+(const Vector3f& v1, const Vector3f& v2);
    friend Vector3f operator-(const Vector3f& v1, const Vector3f& v2);
    friend bool operator==(const Vector3f& v1, const Vector3f& v2);
    friend bool operator!=(const Vector3f& v1, const Vector3f& v2);
    friend Vector3f operator*(const float scale, const Vector3f& v);
    friend Vector3f operator*(const Vector3f& v1, const Vector3f& v2);

    Vector3f& operator += (const Vector3f& that);
    Vector3f& operator *= (const float scale);
    Vector3f& operator -= (const Vector3f& that);

    Vector3f operator-() const;
    operator std::string() const;

    /*
      PUBLIC METHODS
     */


    float Length()const;

    Vector3f& Normalize();
    Vector3f Normalize()const;




    /*
      Rotate this vector around the axis.
     */
    Vector3f& Rotate(const float angle, const Vector3f& axis);

    /*
      Add this vector to an array of floats.
     */
    void Add(std::vector<float>& v) const;

    /*
      STATIC METHODS
     */

    static Vector3f Cross(const Vector3f& v1, const Vector3f& v2);

    static float Dot(const Vector3f& v1, const Vector3f& v2);

    static Vector3f Normalize(const Vector3f& v);


};


/*
  OPERATOR DEFINITIONS
 */

inline Vector3f operator+(const Vector3f& v1, const Vector3f& v2) {
    return Vector3f(
	v1.x+v2.x,
	v1.y+v2.y,
	v1.z+v2.z);
}

inline Vector3f operator-(const Vector3f& v1, const Vector3f& v2) {
    return v1 + (-v2);
}



inline bool operator!=(const Vector3f& v1, const Vector3f& v2) {
    return !(v1 == v2);
}


inline Vector3f operator*(const float scale, const Vector3f& v) {
    return Vector3f(
	scale * v.x,
	scale * v.y,
	scale * v.z);
}

inline Vector3f& Vector3f::operator += (const Vector3f& that){
    *this = (*this) + that;
    return (*this);
}

inline Vector3f& Vector3f::operator -= (const Vector3f& that){
    *this = (*this) - that;
    return (*this);
}

inline Vector3f Vector3f::operator-() const {
    Vector3f v(-x,-y,-z);
    return v;
}

inline Vector3f& Vector3f::operator *= (const float scale) {
    *this = scale * (*this);
    return (*this);
}

inline Vector3f operator*(const Vector3f& v1, const Vector3f& v2) {
    return Vector3f(
	v1.x * v2.x,
	v1.y * v2.y,
	v1.z * v2.z);
}
