#pragma once

class ShaderProgram;
class VBO;
class Camera;

#include "ewa/gl/gl_common.hpp"
#include "ewa/geometry_object.hpp"

class Grass{

    private:

    /*
      INSTANCE VARIABLES.
    */

    GLushort m_numTriangles;

    ShaderProgram* m_shader;

    VBO* m_vertexBuffer;
    VBO* m_indexBuffer;

public:

    //
    Grass();
    ~Grass();

    void Draw(const Camera& camera, const Vector4f& lightPosition);
};
