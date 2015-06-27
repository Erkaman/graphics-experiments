#include "matrix4f.hpp"

#include "math_common.hpp"

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
