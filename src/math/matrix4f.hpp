#pragma once

#include <string>

class Vector3f;
class Vector4f;

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


    friend Matrix4f operator+(const Matrix4f& a, const Matrix4f& b);

    friend Matrix4f operator-(const Matrix4f& a, const Matrix4f& b);

    friend Matrix4f operator*(const float scale, const Matrix4f& m);
    friend Matrix4f operator*(const Matrix4f& m, const float scale);
    friend Vector4f operator*(const Matrix4f& a, const Vector4f& b);
    friend Matrix4f operator*(const Matrix4f& a, const Matrix4f& b);

    friend Matrix4f operator/(const Matrix4f& m, const float div);


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

    static Matrix4f GetNormalMatrix(const Matrix4f& modelViewMatrix);

};
