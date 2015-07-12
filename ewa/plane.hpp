#pragma once

class ShaderProgram;
class VBO;
class Camera;
class Texture;
class Vector4f;

#include "gl/gl_common.hpp"
#include "math/vector3f.hpp"

class ShaderProgram;
class VBO;
class PerlinSeed;


class Plane {

    private:

    /*
      INSTANCE VARIABLES.
    */

    GLushort m_numTriangles;

    ShaderProgram* m_noiseShader;

    Vector3f m_position;

    VBO* m_vertexBuffer;
    VBO* m_indexBuffer;

    PerlinSeed* m_perlinSeed;


public:

    Plane(const Vector3f& position);
    ~Plane();


    void Draw(const Camera& camera, const Vector4f& lightPosition);

};
