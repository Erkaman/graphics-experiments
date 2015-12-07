#include "view_frustum.hpp"

#include "math/matrix4f.hpp"
#include <math.h>

ViewFrustum::ViewFrustum() {

}


void ViewFrustum::Update(const Matrix4f& vp) {

    // right plane
    // take the 3rd row minus 0th row.
    m_frustum[0][0] = vp.m30 - vp.m00;
    m_frustum[0][1] = vp.m31 - vp.m01;
    m_frustum[0][2] = vp.m32 - vp.m02;
    m_frustum[0][3] = vp.m33 - vp.m03;

    // normalize
    float t = (float)sqrt( m_frustum[0][0] * m_frustum[0][0] + m_frustum[0][1] * m_frustum[0][1] + m_frustum[0][2] * m_frustum[0][2] );
    m_frustum[0][0] /= t;
    m_frustum[0][1] /= t;
    m_frustum[0][2] /= t;
    m_frustum[0][3] /= t;



    // left plane.
    // 3rd row plus 0th row.
    m_frustum[1][0] = vp.m30 + vp.m00;
    m_frustum[1][1] = vp.m31 + vp.m01;
    m_frustum[1][2] = vp.m32 + vp.m02;
    m_frustum[1][3] = vp.m33 + vp.m03;

    t = (float)sqrt( m_frustum[1][0] * m_frustum[1][0] + m_frustum[1][1] * m_frustum[1][1] + m_frustum[1][2] * m_frustum[1][2] );
    m_frustum[1][0] /= t;
    m_frustum[1][1] /= t;
    m_frustum[1][2] /= t;
    m_frustum[1][3] /= t;



    // bottom plane.
    //  3rd row plus 1th row.
    m_frustum[2][0] = vp.m30 + vp.m10;
    m_frustum[2][1] = vp.m31 + vp.m11;
    m_frustum[2][2] = vp.m32 + vp.m12;
    m_frustum[2][3] = vp.m33 + vp.m13;

    t = (float)sqrt( m_frustum[2][0] * m_frustum[2][0] + m_frustum[2][1] * m_frustum[2][1] + m_frustum[2][2] * m_frustum[2][2] );
    m_frustum[2][0] /= t;
    m_frustum[2][1] /= t;
    m_frustum[2][2] /= t;
    m_frustum[2][3] /= t;

    // top plane.
    //  3rd row minus 1th row
    m_frustum[3][0] = vp.m30 - vp.m10;
    m_frustum[3][1] = vp.m31 - vp.m11;
    m_frustum[3][2] = vp.m32 - vp.m12;
    m_frustum[3][3] = vp.m33 - vp.m13;


    /* Normalize the result */
    t = (float)sqrt( m_frustum[3][0] * m_frustum[3][0] + m_frustum[3][1] * m_frustum[3][1] + m_frustum[3][2] * m_frustum[3][2] );
    m_frustum[3][0] /= t;
    m_frustum[3][1] /= t;
    m_frustum[3][2] /= t;
    m_frustum[3][3] /= t;


    // far plane.
    // 3rd row minus 2nd row
    m_frustum[4][0] = vp.m30 - vp.m20;
    m_frustum[4][1] = vp.m31 - vp.m21;
    m_frustum[4][2] =  vp.m32 - vp.m22;
    m_frustum[4][3] =  vp.m33 - vp.m23;

    t = (float)sqrt( m_frustum[4][0] * m_frustum[4][0] + m_frustum[4][1] * m_frustum[4][1] + m_frustum[4][2] * m_frustum[4][2] );
    m_frustum[4][0] /= t;
    m_frustum[4][1] /= t;
    m_frustum[4][2] /= t;
    m_frustum[4][3] /= t;

    // near plane.
    // 3rd row + 2nd row
    m_frustum[5][0] = vp.m30 + vp.m20;
    m_frustum[5][1] = vp.m31 + vp.m21;
    m_frustum[5][2] =  vp.m32 + vp.m22;
    m_frustum[5][3] =  vp.m33 + vp.m23;

    t = (float)sqrt( m_frustum[5][0] * m_frustum[5][0] + m_frustum[5][1] * m_frustum[5][1] + m_frustum[5][2] * m_frustum[5][2] );
    m_frustum[5][0] /= t;
    m_frustum[5][1] /= t;
    m_frustum[5][2] /= t;
    m_frustum[5][3] /= t;

}

bool ViewFrustum::IsCuboidInFrustum( const float x, const float y, const float z, const float xSize, const float ySize, const float zSize )
{
    int p;

    for( p = 0; p < 6; p++ )
    {
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
