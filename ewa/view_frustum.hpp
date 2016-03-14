#pragma once

#include "math/vector3f.hpp"

class Matrix4f;
class AABB;

class ViewFrustum {
private:
    // stores the six planes used to represent the view frustrum.
    float m_frustum[6][4];

    Vector3f m_position;

    Vector3f IntersectPlanes(int p1, int p2, int p3)const;

public:

    ViewFrustum();

    void Update(const Matrix4f& vp, const Vector3f& position);

    bool IsAABBInFrustum(const AABB& aabb )const;

    bool IsCuboidInFrustum(
	const float x, const float y, const float z,
	const float xSize, const float ySize, const float zSize )const;

    bool IsSphereInFrustum(
	const float x, const float y, const float z,
	const float radius )const;


    Vector3f lbf()const;
    Vector3f ltf()const;
    Vector3f rbf()const;
    Vector3f rtf()const;

    Vector3f lbn()const;
    Vector3f ltn()const;
    Vector3f rbn()const;
    Vector3f rtn()const;

    Vector3f GetPosition()const;

};
