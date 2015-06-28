#pragma once

#include <string>

class Vector3f;

class Matrix4f {

public:

    /*
      INSTANCE VARIABLES
     */

    float m00, m01, m02, m03,
      	m10, m11, m12, m13,
	m20, m21, m22, m23,
	m30, m31, m32, m33;

    /*
      CONSTRUCTORS:
     */

    Matrix4f();

    Matrix4f(const float m00_, const float m01_, const float m02_, const float m03_,
	     const float m10_, const float m11_, const float m12_, const float m13_,
	     const float m20_, const float m21_, const float m22_, const float m23_,
	     const float m30_, const float m31_, const float m32_, const float m33_);

    /*
      OPERATOR OVERLOADS
     */

    friend bool operator==(const Matrix4f& m1, const Matrix4f& m2);
    friend Matrix4f operator*(const Matrix4f& a, const Matrix4f& b);
    friend Matrix4f operator+(const Matrix4f& a, const Matrix4f& b);
    friend Matrix4f operator*(const float scale, const Matrix4f& m);
    friend Matrix4f operator*(const Matrix4f& m, const float scale);
    friend Matrix4f operator/(const Matrix4f& m, const float div);
    friend Matrix4f operator-(const Matrix4f& a, const Matrix4f& b);

    operator std::string() const;

    /*
      INSTANCE METHODS
     */

    float Determinant() const;
    float Trace() const;
    Matrix4f& Transpose();
    Matrix4f& Inverse();

    /*
      STATIC METHODS
    */

    static Matrix4f CreateIdentity();
    static Matrix4f CreateTranslation(const float x, const float y, const float z);
    static Matrix4f CreateTranslation(const Vector3f& trans);
    static Matrix4f CreatePerspective( const float fov, const float aspectRatio, const float near, const float far);
    static Matrix4f CreateLookAt(const Vector3f& eye, const Vector3f& center, const Vector3f& up);

};


/*

   m00, m[0]

   m01, m[1]
   m02, m[2]
   m03, m[3]

   m10, m[4]
   m11, m[5]
   m12, m[6]
   m13, m[7]

   m20, m[8]
   m21, m[9]
   m22, m[10]
   m23, m[11]

   m30, m[12]
   m31, m[13]
   m32, m[14]
   m33; m[15P]
*/
