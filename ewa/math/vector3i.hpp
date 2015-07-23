#pragma once

#include <string>
#include <vector>

class Vector3i {

public:

/*
  INSTANCE VARIABLES
 */

    int x,y,z;


public:

    /*
      CONSTRUCTORS
     */

    Vector3i():x(0),y(0.),z(0) {  }

    Vector3i(const int value_): x(value_), y(value_),z(value_){  }

    Vector3i(const int x_, const int y_, const int z_): x(x_),y(y_),z(z_){ }

    Vector3i(const Vector3i& that): x(that.x), y(that.y), z(that.z){ }

    /*
      OPERATOR OVERLOADS
     */

    friend Vector3i operator+(const Vector3i& v1, const Vector3i& v2);
    friend Vector3i operator-(const Vector3i& v1, const Vector3i& v2);
    friend bool operator==(const Vector3i& v1, const Vector3i& v2);
    friend bool operator!=(const Vector3i& v1, const Vector3i& v2);
    friend Vector3i operator*(const int scale, const Vector3i& v);
    Vector3i& operator += (const Vector3i& that);
    Vector3i& operator *= (const int scale);
    Vector3i& operator -= (const Vector3i& that);

    Vector3i operator-() const;
    operator std::string() const;

    /*
      PUBLIC METHODS
     */


    float Length()const;

    /*
      STATIC METHODS
     */
    static Vector3i Cross(const Vector3i& v1, const Vector3i& v2);

};


/*
  OPERATOR DEFINITIONS
 */

inline Vector3i operator+(const Vector3i& v1, const Vector3i& v2) {
    return Vector3i(
	v1.x+v2.x,
	v1.y+v2.y,
	v1.z+v2.z);
}

inline Vector3i operator-(const Vector3i& v1, const Vector3i& v2) {
    return v1 + (-v2);
}



inline bool operator!=(const Vector3i& v1, const Vector3i& v2) {
    return !(v1 == v2);
}


inline Vector3i operator*(const int scale, const Vector3i& v) {
    return Vector3i(
	scale * v.x,
	scale * v.y,
	scale * v.z);
}

inline Vector3i& Vector3i::operator += (const Vector3i& that){
    *this = (*this) + that;
    return (*this);
}

inline Vector3i& Vector3i::operator -= (const Vector3i& that){
    *this = (*this) - that;
    return (*this);
}

inline Vector3i Vector3i::operator-() const {
    Vector3i v(-x,-y,-z);
    return v;
}

inline Vector3i& Vector3i::operator *= (const int scale) {
    *this = scale * (*this);
    return (*this);
}
