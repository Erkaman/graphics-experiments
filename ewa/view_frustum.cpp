#include "view_frustum.hpp"

#include "math/matrix4f.hpp"
#include "aabb.hpp"
#include "common.hpp"

#include <math.h>

enum Plane {
    RIGHT=0,
    LEFT=1,
    BOTTOM=2,
    TOP=3,
    FAR=4,
    NEAR=5
};

ViewFrustum::ViewFrustum() { }

// determinant of 3x3 matrix.
float Determinant(
    float a,float b,float c,
    float d,float e,float f,
    float g,float h,float i
    ) {

    return  a*e*i+b*f*g+c*d*h-c*e*g-b*d*i-a*f*h;
}

/*
  Computes the intersection of three planes.
  It is assumed that the intersection between them always exist.
  We do not handle any other cases.
 */
Vector3f Intersect(float* p1, float* p2, float* p3) {

    // We use Cramer's rule to find the intersection.

    float div = Determinant(
	p1[0], p1[1], p1[2],
	p2[0], p2[1], p2[2],
	p3[0], p3[1], p3[2]
	);

    float x = Determinant(
	-p1[3], p1[1], p1[2],
	-p2[3], p2[1], p2[2],
	-p3[3], p3[1], p3[2]
	) / div;

    float y = Determinant(
	p1[0], -p1[3], p1[2],
	p2[0], -p2[3], p2[2],
	p3[0], -p3[3], p3[2]
	) / div;

    float z = Determinant(
	p1[0], p1[1], -p1[3],
	p2[0], p2[1], -p2[3],
	p3[0], p3[1], -p3[3]
	) / div;

    return Vector3f(x,y,z);

}

void ViewFrustum::Update(const Matrix4f& vp) {



    // right plane
    // take the 3rd row minus 0th row.
    m_frustum[RIGHT][0] = vp.m30 - vp.m00;
    m_frustum[RIGHT][1] = vp.m31 - vp.m01;
    m_frustum[RIGHT][2] = vp.m32 - vp.m02;
    m_frustum[RIGHT][3] = vp.m33 - vp.m03;

    // normalize
    float t = (float)sqrt( m_frustum[RIGHT][0] * m_frustum[RIGHT][0] + m_frustum[RIGHT][1] * m_frustum[RIGHT][1] + m_frustum[RIGHT][2] * m_frustum[RIGHT][2] );
    m_frustum[RIGHT][0] /= t;
    m_frustum[RIGHT][1] /= t;
    m_frustum[RIGHT][2] /= t;
    m_frustum[RIGHT][3] /= t;

    // left plane.
    // 3rd row plus 0th row.
    m_frustum[LEFT][0] = vp.m30 + vp.m00;
    m_frustum[LEFT][1] = vp.m31 + vp.m01;
    m_frustum[LEFT][2] = vp.m32 + vp.m02;
    m_frustum[LEFT][3] = vp.m33 + vp.m03;

    t = (float)sqrt( m_frustum[LEFT][0] * m_frustum[LEFT][0] + m_frustum[LEFT][1] * m_frustum[LEFT][1] + m_frustum[LEFT][2] * m_frustum[LEFT][2] );
    m_frustum[LEFT][0] /= t;
    m_frustum[LEFT][1] /= t;
    m_frustum[LEFT][2] /= t;
    m_frustum[LEFT][3] /= t;



    // bottom plane.
    //  3rd row plus 1th row.
    m_frustum[BOTTOM][0] = vp.m30 + vp.m10;
    m_frustum[BOTTOM][1] = vp.m31 + vp.m11;
    m_frustum[BOTTOM][2] = vp.m32 + vp.m12;
    m_frustum[BOTTOM][3] = vp.m33 + vp.m13;

    t = (float)sqrt( m_frustum[BOTTOM][0] * m_frustum[BOTTOM][0] + m_frustum[BOTTOM][1] * m_frustum[BOTTOM][1] + m_frustum[BOTTOM][2] * m_frustum[BOTTOM][2] );
    m_frustum[BOTTOM][0] /= t;
    m_frustum[BOTTOM][1] /= t;
    m_frustum[BOTTOM][2] /= t;
    m_frustum[BOTTOM][3] /= t;

    // top plane.
    //  3rd row minus 1th row
    m_frustum[TOP][0] = vp.m30 - vp.m10;
    m_frustum[TOP][1] = vp.m31 - vp.m11;
    m_frustum[TOP][2] = vp.m32 - vp.m12;
    m_frustum[TOP][3] = vp.m33 - vp.m13;


    /* Normalize the result */
    t = (float)sqrt( m_frustum[TOP][0] * m_frustum[TOP][0] + m_frustum[TOP][1] * m_frustum[TOP][1] + m_frustum[TOP][2] * m_frustum[TOP][2] );
    m_frustum[TOP][0] /= t;
    m_frustum[TOP][1] /= t;
    m_frustum[TOP][2] /= t;
    m_frustum[TOP][3] /= t;


    // far plane.
    // 3rd row minus 2nd row
    m_frustum[FAR][0] = vp.m30 - vp.m20;
    m_frustum[FAR][1] = vp.m31 - vp.m21;
    m_frustum[FAR][2] =  vp.m32 - vp.m22;
    m_frustum[FAR][3] =  vp.m33 - vp.m23;

    t = (float)sqrt( m_frustum[FAR][0] * m_frustum[FAR][0] + m_frustum[FAR][1] * m_frustum[FAR][1] + m_frustum[FAR][2] * m_frustum[FAR][2] );
    m_frustum[FAR][0] /= t;
    m_frustum[FAR][1] /= t;
    m_frustum[FAR][2] /= t;
    m_frustum[FAR][3] /= t;

    // near plane.
    // 3rd row + 2nd row
    m_frustum[NEAR][0] = vp.m30 + vp.m20;
    m_frustum[NEAR][1] = vp.m31 + vp.m21;
    m_frustum[NEAR][2] =  vp.m32 + vp.m22;
    m_frustum[NEAR][3] =  vp.m33 + vp.m23;

    t = (float)sqrt( m_frustum[NEAR][0] * m_frustum[NEAR][0] + m_frustum[NEAR][1] * m_frustum[NEAR][1] + m_frustum[NEAR][2] * m_frustum[NEAR][2] );
    m_frustum[NEAR][0] /= t;
    m_frustum[NEAR][1] /= t;
    m_frustum[NEAR][2] /= t;
    m_frustum[NEAR][3] /= t;

    /*
    LOG_I("update: %s", tocstr(Intersect(
				   m_frustum[NEAR], // near
				   m_frustum[TOP], // top
				   m_frustum[RIGHT] // left

				   )) );
    exit(1);
    */

}

// solve for left, near, bottom.

bool ViewFrustum::IsCuboidInFrustum( const float x, const float y, const float z, const float xSize, const float ySize, const float zSize )const
{
    int p;

    for( p = 0; p < 6; p++ )
    {

	/*
	  Let f be the equation of plane m_frustum[p]. We plug the corners of the cuboid into f,
	  and if all corners have the same sign, and then the cuboid must be on the other side of the plane, and collision is not possible.
	 */

	if( m_frustum[p][0] * (x - xSize) + m_frustum[p][1] * (y - ySize) + m_frustum[p][2] * (z - zSize) + m_frustum[p][3] > 0 )
	    continue;
	if( m_frustum[p][0] * (x + xSize) + m_frustum[p][1] * (y - ySize) + m_frustum[p][2] * (z - zSize) + m_frustum[p][3] > 0 )
	    continue;
	if( m_frustum[p][0] * (x - xSize) + m_frustum[p][1] * (y + ySize) + m_frustum[p][2] * (z - zSize) + m_frustum[p][3] > 0 )
	    continue;
	if( m_frustum[p][0] * (x + xSize) + m_frustum[p][1] * (y + ySize) + m_frustum[p][2] * (z - zSize) + m_frustum[p][3] > 0 )
	    continue;
	if( m_frustum[p][0] * (x - xSize) + m_frustum[p][1] * (y - ySize) + m_frustum[p][2] * (z + zSize) + m_frustum[p][3] > 0 )
	    continue;
	if( m_frustum[p][0] * (x + xSize) + m_frustum[p][1] * (y - ySize) + m_frustum[p][2] * (z + zSize) + m_frustum[p][3] > 0 )
	    continue;
	if( m_frustum[p][0] * (x - xSize) + m_frustum[p][1] * (y + ySize) + m_frustum[p][2] * (z + zSize) + m_frustum[p][3] > 0 )
	    continue;
	if( m_frustum[p][0] * (x + xSize) + m_frustum[p][1] * (y + ySize) + m_frustum[p][2] * (z + zSize) + m_frustum[p][3] > 0 )
	    continue;
	return false;
    }
    return true;
}

bool ViewFrustum::IsAABBInFrustum(const AABB& aabb )const {
    Vector3f center = (aabb.min + aabb.max) * 0.5f;

    Vector3f radius = (aabb.max - center);

    return IsCuboidInFrustum(
	center.x, center.y, center.z,
	radius.x,radius.y,radius.z
	);

}
