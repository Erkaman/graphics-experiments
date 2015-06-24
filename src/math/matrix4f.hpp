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

    void SetIdentity(){
	m01 = m02 = m03 = m10 = m12 = m13 = m20 = m21 = m23 = m30 = m31 = m32= 0.0f;
	m11 = m00 =  m22 = m33 = 1.0f;
    }

    void SetTranslation(const float x, const float y, const float z){
	SetIdentity();

	m03 = x;
	m13 = y;
	m23 = z;
    }

    operator std::string() const {

	return
	    std::to_string(m00) + ", " + std::to_string(m01) + ", " + std::to_string(m02) + ", " + std::to_string(m03) + "\n" +
	    std::to_string(m10) + ", " + std::to_string(m11) + ", " + std::to_string(m12) + ", " + std::to_string(m13) + "\n" +
	    std::to_string(m20) + ", " + std::to_string(m21) + ", " + std::to_string(m22) + ", " + std::to_string(m23) + "\n" +
	    std::to_string(m30) + ", " + std::to_string(m31) + ", " + std::to_string(m32) + ", " + std::to_string(m33) + "\n";
    }
};
