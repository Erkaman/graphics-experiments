#pragma once

#include <string>
#include <vector>

class Vector2i {

public:

/*
  INSTANCE VARIABLES
 */

    int x,y;

public:

    /*
      CONSTRUCTORS
     */

    Vector2i():x(0),y(0.) {  }

    Vector2i(const int value_): x(value_), y(value_){  }

    Vector2i(const int x_, const int y_): x(x_),y(y_){ }

    Vector2i(const Vector2i& that): x(that.x), y(that.y) { }

    /*
      OPERATOR OVERLOADS
     */

    friend Vector2i operator+(const Vector2i& v1, const Vector2i& v2);
    friend Vector2i operator-(const Vector2i& v1, const Vector2i& v2);
    friend bool operator==(const Vector2i& v1, const Vector2i& v2);
    friend bool operator!=(const Vector2i& v1, const Vector2i& v2);
    friend Vector2i operator*(const int scale, const Vector2i& v);
    Vector2i& operator += (const Vector2i& that);
    Vector2i& operator *= (const int scale);
    Vector2i& operator -= (const Vector2i& that);

    Vector2i operator-() const;
    operator std::string() const;

    /*
      PUBLIC METHODS
     */


    float Length()const;

    /*
      STATIC METHODS
     */
};

namespace std {
template <> struct hash<Vector2i> {
    size_t operator()(const Vector2i& v) const{
	return 997 * (v.x) ^ 991 * (v.y);
    }
};
}


/*
  OPERATOR DEFINITIONS
 */

inline Vector2i operator+(const Vector2i& v1, const Vector2i& v2) {
    return Vector2i(
	v1.x+v2.x,
	v1.y+v2.y);
}

inline Vector2i operator-(const Vector2i& v1, const Vector2i& v2) {
    return v1 + (-v2);
}



inline bool operator!=(const Vector2i& v1, const Vector2i& v2) {
    return !(v1 == v2);
}


inline Vector2i operator*(const int scale, const Vector2i& v) {
    return Vector2i(
	scale * v.x,
	scale * v.y);
}

inline Vector2i& Vector2i::operator += (const Vector2i& that){
    *this = (*this) + that;
    return (*this);
}

inline Vector2i& Vector2i::operator -= (const Vector2i& that){
    *this = (*this) - that;
    return (*this);
}

inline Vector2i Vector2i::operator-() const {
    Vector2i v(-x,-y);
    return v;
}

inline Vector2i& Vector2i::operator *= (const int scale) {
    *this = scale * (*this);
    return (*this);
}
