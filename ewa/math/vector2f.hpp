#pragma once

#include <string>
#include <vector>

class Vector2f {

public:

    /*
      INSTANCE VARIABLES:
     */

    float x,y;

    /*
      CONSTRUCTORS:
     */
    Vector2f(const float x_, const float y_);
    Vector2f(const float x_);

    Vector2f();

    /*
      OPERATOR OVERLOADS
     */


    operator std::string() const;

    friend Vector2f operator+(const Vector2f& v1, const Vector2f& v2);
    friend Vector2f operator-(const Vector2f& v1, const Vector2f& v2);
    friend bool operator==(const Vector2f& v1, const Vector2f& v2);
    friend bool operator!=(const Vector2f& v1, const Vector2f& v2);
    friend Vector2f operator*(const float scale, const Vector2f& v);
    friend Vector2f operator*(const Vector2f& v, const float scale);
    Vector2f& operator += (const Vector2f& that);
    Vector2f& operator *= (const float scale);
    Vector2f& operator -= (const Vector2f& that);
    Vector2f operator-() const;

    void Add(std::vector<float>& v)const;

    /*
      PUBLIC METHODS
     */
    float Length()const;

    Vector2f& Normalize();
    Vector2f Normalize()const;

};

// hashcode
namespace std {
template <> struct hash<Vector2f> {
    size_t operator()(const Vector2f& v) const{
	return 997 * ((int)v.x) ^ 991 * ((int)v.y);
    }
};
}


/*
  OPERATOR DEFINITIONS
 */

inline Vector2f operator+(const Vector2f& v1, const Vector2f& v2) {
    return Vector2f(
	v1.x+v2.x,
	v1.y+v2.y);
}

inline Vector2f operator-(const Vector2f& v1, const Vector2f& v2) {
    return v1 + (-v2);
}



inline bool operator!=(const Vector2f& v1, const Vector2f& v2) {
    return !(v1 == v2);
}


inline Vector2f operator*(const float scale, const Vector2f& v) {
    return Vector2f(
	scale * v.x,
	scale * v.y);
}

inline Vector2f operator*(const Vector2f& v, const float scale) {
    return scale * v;
}

inline Vector2f& Vector2f::operator += (const Vector2f& that){
    *this = (*this) + that;
    return (*this);
}

inline Vector2f& Vector2f::operator -= (const Vector2f& that){
    *this = (*this) - that;
    return (*this);
}

inline Vector2f Vector2f::operator-() const {
    Vector2f v(-x,-y);
    return v;
}

inline Vector2f& Vector2f::operator *= (const float scale) {
    *this = scale * (*this);
    return (*this);
}
