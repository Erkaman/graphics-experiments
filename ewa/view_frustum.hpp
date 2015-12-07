#pragma once

class Matrix4f;

class ViewFrustum {
private:
    // stores the six planes used to represent the view frustrum.
    float m_frustum[6][4];

public:

    ViewFrustum();


    void Update(const Matrix4f& vp);

    bool IsCuboidInFrustum(
	const float x, const float y, const float z,
	const float xSize, const float ySize, const float zSize );

};
