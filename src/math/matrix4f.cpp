#include "matrix4f.hpp"

#include "math_common.hpp"

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
