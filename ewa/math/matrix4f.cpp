#include "matrix4f.hpp"

#include "math_common.hpp"
#include "vector3f.hpp"
#include "vector4f.hpp"
#include "log.hpp"

using std::string;

Matrix4f Matrix4f::CreatePerspective( const float fov, const float aspectRatio, const float near, const float far) {

    float ymax = near * tan(fov * PI/360.0f);
    float xmax = ymax * aspectRatio;


    float left = -xmax;
    float right = +xmax;

    float bottom = -ymax;
    float top = +ymax;


    float f1, f2, f3, f4;
    f1 = 2.0f * near;
    f2 = right - left;
    f3 = top - bottom;
    f4 = far - near;

    // http://www.songho.ca/opengl/gl_projectionmatrix.html

    return Matrix4f(
	f1/f2, 0.0f, (right + left)/f2, 0.0f,
	0.0f, f1/f3, (top + bottom)/ f3, 0.0f,
	0.0f, 0.0f, (-far - near)/f4,  (-far*f1)/f4 ,
	0.0f, 0.0f,      -1.0f  , 0.0f);

}


Matrix4f Matrix4f::CreateLookAt(const Vector3f& eye, const Vector3f& center, const Vector3f& up) {
    Vector3f zaxis = (eye - center).Normalize(); // forward vector.
    Vector3f xaxis = (Vector3f::Cross(up, zaxis)).Normalize(); // right vector.
    Vector3f yaxis = Vector3f::Cross(zaxis, xaxis).Normalize(); // right vector.

    Matrix4f orientation(
	xaxis.x, xaxis.y, xaxis.z, 0,
	yaxis.x, yaxis.y, yaxis.z, 0 ,
	zaxis.x, zaxis.y, zaxis.z, 0 ,
	0,       0,       0,     1
	);

    Vector3f eyeNeg = -eye;

    Matrix4f translation = Matrix4f::CreateTranslation(eyeNeg);
    Matrix4f m = orientation * translation;

    return m;

}

bool operator==(const Matrix4f& m1, const Matrix4f& m2) {
    return
	FloatEquals(m1.m00, m2.m00) && FloatEquals(m1.m01 , m2.m01) && FloatEquals(m1.m02 , m2.m02) && FloatEquals(m1.m03 , m2.m03) &&
	FloatEquals(m1.m10 , m2.m10) && FloatEquals(m1.m11 , m2.m11) && FloatEquals(m1.m12 , m2.m12) && FloatEquals(m1.m13 , m2.m13) &&
	FloatEquals(m1.m20 , m2.m20) && FloatEquals(m1.m21 , m2.m21) && FloatEquals(m1.m22 , m2.m22) && FloatEquals(m1.m23 , m2.m23) &&
	FloatEquals(m1.m30 , m2.m30) && FloatEquals(m1.m31 , m2.m31) && FloatEquals(m1.m32 , m2.m32) && FloatEquals(m1.m33 , m2.m33);
}

Matrix4f Matrix4f::CreateTranslation(const Vector3f& trans){
    return CreateTranslation(trans.x,trans.y,trans.z);
}

static float Determinant3x3(
    float a,float b,float c,
    float d,float e,float f,
    float g,float h,float i) {
    return a*e*i+b*f*g+c*d*h-c*e*g-b*d*i-a*f*h ;
}

float Matrix4f::Determinant() const {
    return
	m00 * Determinant3x3(
	    m11, m12, m13,
	    m21, m22, m23,
	    m31, m32, m33)
	-
	m01 * Determinant3x3(

	    m10, m12, m13,
	    m20, m22, m23,
	    m30, m32, m33)
	+
	m02 * Determinant3x3(

	    m10, m11, m13,
	    m20, m21, m23,
	    m30, m31, m33)
	-
	m03 * Determinant3x3(
	    m10, m11, m12,
	    m20, m21, m22,
	    m30, m31, m32);
}


float Matrix4f::Trace() const {
    return m00 + m11 + m22 + m33;
}


Matrix4f::Matrix4f():
	m00(0), m01(0), m02(0), m03(0),
	m10(0), m11(0), m12(0), m13(0),
	m20(0), m21(0), m22(0), m23(0),
	m30(0), m31(0), m32(0), m33(0) { }



Matrix4f::Matrix4f(const float m00_, const float m01_, const float m02_, const float m03_,
		   const float m10_, const float m11_, const float m12_, const float m13_,
		   const float m20_, const float m21_, const float m22_, const float m23_,
		   const float m30_, const float m31_, const float m32_, const float m33_):
    m00(m00_), m01(m01_), m02(m02_), m03(m03_),
    m10(m10_), m11(m11_), m12(m12_), m13(m13_),
    m20(m20_), m21(m21_), m22(m22_), m23(m23_),
    m30(m30_), m31(m31_), m32(m32_), m33(m33_) { }


Matrix4f operator*(const Matrix4f& a, const Matrix4f& b) {
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


Matrix4f::operator std::string() const {

	return
	    "{\n" +
	    std::to_string(m00) + ", " + std::to_string(m01) + ", " + std::to_string(m02) + ", " + std::to_string(m03) + "\n" +
	    std::to_string(m10) + ", " + std::to_string(m11) + ", " + std::to_string(m12) + ", " + std::to_string(m13) + "\n" +
	    std::to_string(m20) + ", " + std::to_string(m21) + ", " + std::to_string(m22) + ", " + std::to_string(m23) + "\n" +
	    std::to_string(m30) + ", " + std::to_string(m31) + ", " + std::to_string(m32) + ", " + std::to_string(m33) + "\n" + "}";
    }

Matrix4f& Matrix4f::Transpose() {
	float new_m01 = m10;
	float new_m02 = m20;
	float new_m03 = m30;

	float new_m10 = m01;
	float new_m12 = m21;
	float new_m13 = m31;

	float new_m20 = m02;
	float new_m21 = m12;
	float new_m23 = m32;

	float new_m30 = m03;
	float new_m31 = m13;
	float new_m32 = m23;

	m01 = new_m01;
	m02 = new_m02;
	m03 = new_m03;

	m10 = new_m10;
	m12 = new_m12;
	m13 = new_m13;

	m20 = new_m20;
	m21 = new_m21;
	m23 = new_m23;

	m30 = new_m30;
	m31 = new_m31;
	m32 = new_m32;

	return *this;
}

Matrix4f Matrix4f::CreateIdentity(){
	return Matrix4f(
	    1.0f, 0.0, 0.0f, 0.0f,
	    0.0f, 1.0, 0.0f, 0.0f,
	    0.0f, 0.0, 1.0f, 0.0f,
	    0.0f, 0.0, 0.0f, 1.0f
	    );
}

Matrix4f Matrix4f::CreateTranslation(const float x, const float y, const float z){
    Matrix4f t = CreateIdentity();

    t.m03 = x;
    t.m13 = y;
    t.m23 = z;

    return t;
}

Matrix4f Matrix4f::CreateScale(const float x, const float y, const float z) {
    Matrix4f s = CreateIdentity();

    s.m00 = x;
    s.m11 = y;
    s.m22 = z;

    return s;
}

Matrix4f Matrix4f::CreateScale(const Vector3f& scale) {
    return CreateScale(scale.x, scale.y, scale.z);
}

Matrix4f operator*(const float scale, const Matrix4f& m){
    return Matrix4f(
	scale*m.m00, scale*m.m01, scale*m.m02, scale*m.m03,
      	scale*m.m10, scale*m.m11, scale*m.m12, scale*m.m13,
	scale*m.m20, scale*m.m21, scale*m.m22, scale*m.m23,
	scale*m.m30, scale*m.m31, scale*m.m32, scale*m.m33

	);
}

Matrix4f operator*(const Matrix4f& m, const float scale) {
    return scale * m;
}

Matrix4f operator/(const Matrix4f& m, const float div) {
    if(FloatEquals(div, 0)) {
	LOG_E("matrix division by zero!");
    }

    float f = 1.0f / div;
    return (f) * m;
}


Matrix4f& Matrix4f::Inverse() {

    float inv[16], det;

    inv[0] = m11  * m22 * m33 -
	m11  * m23 * m32 -
	m21  * m12  * m33 +
	m21  * m13  * m32 +
	m31 * m12  * m23 -
	m31 * m13  * m22;

    inv[4] = -m10  * m22 * m33 +
	m10  * m23 * m32 +
	m20  * m12  * m33 -
	m20  * m13  * m32 -
	m30 * m12  * m23 +
	m30 * m13  * m22;

    inv[8] = m10  * m21 * m33 -
	m10  * m23 * m31 -
	m20  * m11 * m33 +
	m20  * m13 * m31 +
	m30 * m11 * m23 -
	m30 * m13 * m21;

    inv[12] = -m10  * m21 * m32 +
	m10  * m22 * m31 +
	m20  * m11 * m32 -
	m20  * m12 * m31 -
	m30 * m11 * m22 +
	m30 * m12 * m21;

    inv[1] = -m01  * m22 * m33 +
	m01  * m23 * m32 +
	m21  * m02 * m33 -
	m21  * m03 * m32 -
	m31 * m02 * m23 +
	m31 * m03 * m22;

    inv[5] = m00  * m22 * m33 -
	m00  * m23 * m32 -
	m20  * m02 * m33 +
	m20  * m03 * m32 +
	m30 * m02 * m23 -
	m30 * m03 * m22;

    inv[9] = -m00  * m21 * m33 +
	m00  * m23 * m31 +
	m20  * m01 * m33 -
	m20  * m03 * m31 -
	m30 * m01 * m23 +
	m30 * m03 * m21;

    inv[13] = m00  * m21 * m32 -
	m00  * m22 * m31 -
	m20  * m01 * m32 +
	m20  * m02 * m31 +
	m30 * m01 * m22 -
	m30 * m02 * m21;

    inv[2] = m01  * m12 * m33 -
	m01  * m13 * m32 -
	m11  * m02 * m33 +
	m11  * m03 * m32 +
	m31 * m02 * m13 -
	m31 * m03 * m12;

    inv[6] = -m00  * m12 * m33 +
	m00  * m13 * m32 +
	m10  * m02 * m33 -
	m10  * m03 * m32 -
	m30 * m02 * m13 +
	m30 * m03 * m12;

    inv[10] = m00  * m11 * m33 -
	m00  * m13 * m31 -
	m10  * m01 * m33 +
	m10  * m03 * m31 +
	m30 * m01 * m13 -
	m30 * m03 * m11;

    inv[14] = -m00  * m11 * m32 +
	m00  * m12 * m31 +
	m10  * m01 * m32 -
	m10  * m02 * m31 -
	m30 * m01 * m12 +
	m30 * m02 * m11;

    inv[3] = -m01 * m12 * m23 +
	m01 * m13 * m22 +
	m11 * m02 * m23 -
	m11 * m03 * m22 -
	m21 * m02 * m13 +
	m21 * m03 * m12;

    inv[7] = m00 * m12 * m23 -
	m00 * m13 * m22 -
	m10 * m02 * m23 +
	m10 * m03 * m22 +
	m20 * m02 * m13 -
	m20 * m03 * m12;

    inv[11] = -m00 * m11 * m23 +
	m00 * m13 * m21 +
	m10 * m01 * m23 -
	m10 * m03 * m21 -
	m20 * m01 * m13 +
	m20 * m03 * m11;

    inv[15] = m00 * m11 * m22 -
	m00 * m12 * m21 -
	m10 * m01 * m22 +
	m10 * m02 * m21 +
	m20 * m01 * m12 -
	m20 * m02 * m11;

    det = m00 * inv[0] + m01 * inv[4] + m02 * inv[8] + m03 * inv[12];

    if (det == 0)
	LOG_E("Determinant() = 0, so the matrix can not be inversed");

    det = 1.0f / det;

    m00 = inv[0] * det;
    m01 = inv[1] * det;
    m02 = inv[2] * det;
    m03 = inv[3] * det;

    m10 = inv[4] * det;
    m11 = inv[5] * det;
    m12 = inv[6] * det;
    m13 = inv[7] * det;

    m20 = inv[8] * det;
    m21 = inv[9] * det;
    m22 = inv[10] * det;
    m23 = inv[11] * det;

    m30 = inv[12] * det;
    m31 = inv[13] * det;
    m32 = inv[14] * det;
    m33 = inv[15] * det;

    return (*this);

}

Matrix4f operator+(const Matrix4f& a, const Matrix4f& b) {
    return Matrix4f(
	a.m00 + b.m00, a.m01 + b.m01, a.m02 + b.m02, a.m03 + b.m03,
      	a.m10 + b.m10, a.m11 + b.m11, a.m12 + b.m12, a.m13 + b.m13,
	a.m20 + b.m20, a.m21 + b.m21, a.m22 + b.m22, a.m23 + b.m23,
	a.m30 + b.m30, a.m31 + b.m31, a.m32 + b.m32, a.m33 + b.m33
	);
}


Matrix4f operator-(const Matrix4f& a, const Matrix4f& b) {
    return Matrix4f(
	a.m00 - b.m00, a.m01 - b.m01, a.m02 - b.m02, a.m03 - b.m03,
      	a.m10 - b.m10, a.m11 - b.m11, a.m12 - b.m12, a.m13 - b.m13,
	a.m20 - b.m20, a.m21 - b.m21, a.m22 - b.m22, a.m23 - b.m23,
	a.m30 - b.m30, a.m31 - b.m31, a.m32 - b.m32, a.m33 - b.m33
	);
}

Vector4f operator*(const Matrix4f& a, const Vector4f& b) {
    return Vector4f(
	a.m00 * b.x + a.m01 * b.y + a.m02 * b.z + a.m03 * b.w,
	a.m10 * b.x + a.m11 * b.y + a.m12 * b.z + a.m13 * b.w,
	a.m20 * b.x + a.m21 * b.y + a.m22 * b.z + a.m23 * b.w,
	a.m30 * b.x + a.m31 * b.y + a.m32 * b.z + a.m33 * b.w);
}

Matrix4f Matrix4f::GetNormalMatrix(const Matrix4f& modelViewMatrix) {

    Matrix4f mv(modelViewMatrix);

//    LOG_I("mv: %s", string(mv).c_str() );

    Matrix4f trans = mv.Transpose();

    // LOG_I("trans: %s", string(trans).c_str() );


    Matrix4f inv = trans.Inverse();

    /*
    LOG_I("inv: %s", string(inv).c_str() );

    exit(1);
    */

    return inv;
}


Matrix4f Matrix4f::CreateOrthographic( const float left, const float right, const float bottom, const float top, const float near, const float far) {

//    return Matrix4f::CreateIdentity();

     return Matrix4f(
	 2.0f/(right-left), 0.0f, 0.0f, -(right + left) / (right - left),
	0.0f, 2.0f / (top - bottom), 0.0f, -(top + bottom) / (top - bottom),
	 0.0f, 0.0f, 2.0f / (far - near) ,  -(far +near) / (far - near),
	 0.0f, 0.0f,      0.0f  , 1.0f);
}

Matrix4f Matrix4f::CreateRotate(const float angle, const Vector3f& axis) {
    const float a = ToRadians(angle);
   const float cosA = cos(a);
    const float oneMinusCosA = 1 - cosA;
    const float sinA  = sin(a);

    // axis must be normalized.
    const Vector3f normAxis = axis.Normalize();


    const float ux = normAxis.x;
    const float uy = normAxis.y;
    const float uz = normAxis.z;

    // we use the following formula:
    // https://en.wikipedia.org/wiki/Rotation_matrix#Rotation_matrix_from_axis_and_angle
    return Matrix4f(
	cosA + ux*ux * (oneMinusCosA),ux * uy * oneMinusCosA - uz * sinA, ux *  uz * (oneMinusCosA) + uy * sinA, 0,
	uy * ux * oneMinusCosA + uz * sinA, cosA + uy * uy * (oneMinusCosA), uy * uz * (oneMinusCosA) - ux * sinA, 0,
	uz * ux * oneMinusCosA  - uy * sinA, uz * uy * oneMinusCosA + ux * sinA, cosA + uz * uz * oneMinusCosA, 0,
	0,0,0, 1
	);

}
