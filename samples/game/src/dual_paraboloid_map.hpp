#pragma once

#include "ewa/gl/fbo.hpp"
#include "ewa/math/matrix4f.hpp"
#include "ewa/math/vector3f.hpp"

class ColorFBO;
class Vector3f;

struct Paraboloid {

public:

    ColorFBO* m_fbo;
    Matrix4f m_viewMatrix;

    Vector3f m_direction;
    Vector3f m_position;

    int m_i;
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
