#pragma once

#include <string>
#include "common.hpp"

class Matrix4f {

public:

    float m00, m01, m02, m03,
      	m10, m11, m12, m13,
	m20, m21, m22, m23,
	m30, m31, m32, m33;

    Matrix4f(){
	m00 = m01 = m02 = m03 = m10 = m11 = m12 = m13 = m20 = m21 = m22 = m23 = m30 = m31 = m32 = m33 = 0.0f;
    }

    Matrix4f(const float m00_, const float m01_, const float m02_, const float m03_,
	     const float m10_, const float m11_, const float m12_, const float m13_,
	     const float m20_, const float m21_, const float m22_, const float m23_,
	     const float m30_, const float m31_, const float m32_, const float m33_):
	m00(m00_), m01(m01_), m02(m02_), m03(m03_),
	m10(m10_), m11(m11_), m12(m12_), m13(m13_),
	m20(m20_), m21(m21_), m22(m22_), m23(m23_),
	m30(m30_), m31(m31_), m32(m32_), m33(m33_) { }


    friend bool operator==(const Matrix4f& m1, const Matrix4f& m2);
    friend Matrix4f operator*(const Matrix4f& a, const Matrix4f& b);

    static Matrix4f CreateIdentity(){
	return Matrix4f(
	    1.0f, 0.0, 0.0f, 0.0f,
	    0.0f, 1.0, 0.0f, 0.0f,
	    0.0f, 0.0, 1.0f, 0.0f,
	    0.0f, 0.0, 0.0f, 1.0f
	    );
    }

    static Matrix4f CreateTranslation(const float x, const float y, const float z){
	Matrix4f t = CreateIdentity();

	t.m03 = x;
	t.m13 = y;
	t.m23 = z;

	return t;
    }

    static Matrix4f CreatePerspective( const float fov, const float aspectRatio, const float near, const float far);

    operator std::string() const {

	return
	    "{\n" +
	    std::to_string(m00) + ", " + std::to_string(m01) + ", " + std::to_string(m02) + ", " + std::to_string(m03) + "\n" +
	    std::to_string(m10) + ", " + std::to_string(m11) + ", " + std::to_string(m12) + ", " + std::to_string(m13) + "\n" +
	    std::to_string(m20) + ", " + std::to_string(m21) + ", " + std::to_string(m22) + ", " + std::to_string(m23) + "\n" +
	    std::to_string(m30) + ", " + std::to_string(m31) + ", " + std::to_string(m32) + ", " + std::to_string(m33) + "\n" + "}";
    }
};


bool operator==(const Matrix4f& m1, const Matrix4f& m2);


inline Matrix4f operator*(const Matrix4f& a, const Matrix4f& b) {
    Matrix4f result;

    result.m00 = a.m00*b.m00 + a.m01*b.m10 + a.m02*b.m20 + a.m03*b.m30;
    result.m01 = a.m00*b.m01 + a.m01*b.m11 + a.m02*b.m21 + a.m03*b.m31;
    result.m02 = a.m00*b.m02 + a.m01*b.m12 + a.m02*b.m22 + a.m03*b.m32;
    result.m03 = a.m00*b.m03 + a.m01*b.m13 + a.m02*b.m23 + a.m03*b.m33;

    result.m10 = a.m10*b.m00 + a.m11*b.m10 + a.m12*b.m20 + a.m13*b.m30;
    result.m11 = a.m10*b.m01 + a.m11*b.m11 + a.m12*b.m21 + a.m13*b.m31;
    result.m12 = a.m10*b.m02 + a.m11*b.m12 + a.m12*b.m22 + a.m13*b.m32;
    result.m13 = a.m10*b.m03 + a.m11*b.m13 + a.m12*b.m23 + a.m13*b.m33;

    result.m20 = a.m20*b.m00 + a.m21*b.m10 + a.m22*b.m20 + a.m23*b.m30;
    result.m21 = a.m20*b.m01 + a.m21*b.m11 + a.m22*b.m21 + a.m23*b.m31;
    result.m22 = a.m20*b.m02 + a.m21*b.m12 + a.m22*b.m22 + a.m23*b.m32;
    result.m23 = a.m20*b.m03 + a.m21*b.m13 + a.m22*b.m23 + a.m23*b.m33;

    result.m30 = a.m30*b.m00 + a.m31*b.m10 + a.m32*b.m20 + a.m33*b.m30;
    result.m31 = a.m30*b.m01 + a.m31*b.m11 + a.m32*b.m21 + a.m33*b.m31;
    result.m32 = a.m30*b.m02 + a.m31*b.m12 + a.m32*b.m22 + a.m33*b.m32;
    result.m33 = a.m30*b.m03 + a.m31*b.m13 + a.m32*b.m23 + a.m33*b.m33;

    return result;
}
