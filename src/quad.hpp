#pragma once

class ShaderProgram;
class VBO;
class Camera;
class Texture;
class Vector4f;
class Vector2f;

#include "gl/gl_common.hpp"


class VBO;

class Quad {

private:

    /*
      INSTANCE VARIABLES.
    */

    GLushort m_numTriangles;

    VBO* m_vertexBuffer;
    VBO* m_indexBuffer;

public:

    Quad(const Vector2f c1, const Vector2f& c2);
    ~Quad();

    void Draw();

};
