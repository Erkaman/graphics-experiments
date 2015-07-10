#pragma once

class ShaderProgram;
class VBO;
class Camera;
class Texture;
class Vector4f;

#include "gl/gl_common.hpp"


class VBO;

class Quad {

    private:

    /*
      INSTANCE VARIABLES.
    */

    GLushort m_numTriangles;

    std::unique_ptr<VBO> m_vertexBuffer;
    std::unique_ptr<VBO> m_indexBuffer;

public:

    Quad();

    void Draw();

};
