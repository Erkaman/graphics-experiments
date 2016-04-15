#pragma once

#include "ewa/gl/fbo.hpp"
#include "ewa/math/matrix4f.hpp"
#include "ewa/math/vector3f.hpp"

class ColorFBO;
class Vector3f;
class AABB;
class ShaderProgram;

struct Paraboloid {

private:

public:



    ColorFBO* m_fbo;
    Matrix4f m_viewMatrix;

    Matrix4f m_fuck;


    Vector3f m_direction;
    Vector3f m_position;

    int m_i;

    bool InFrustum(const Vector3f& position)const;
    bool InFrustum(const AABB& aabb)const;

    void SetParaboloidUniforms(
    ShaderProgram& shader,
    const Matrix4f& modelMatrix, const Matrix4f& viewMatrix,
    const Matrix4f& projectionMatrix, const Vector3f&
    cameraPosition, const Vector4f& lightDirection
	) const;

};

class DualParaboloidMap {

private:

    /*
      First paraboloid is facing (0,0,1), and second one is facing (0,0,-1).
     */
    Paraboloid m_paraboloids[2];

    size_t m_size;

public:

    DualParaboloidMap();

    void Update(const Vector3f& carPosition);

    Paraboloid GetParaboloid(int i);

    size_t GetSize();




};
