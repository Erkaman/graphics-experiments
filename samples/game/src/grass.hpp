#pragma once

#include "ewa/gl/gl_common.hpp"


class VBO;
class ShaderProgram;
class Camera;
class Texture;
class Vector2f;

class Grass{

private:

    unsigned int m_grassNumTriangles;
    /*
      These are used to draw the billboard sun:
     */
    VBO* m_grassVertexBuffer;
    VBO* m_grassIndexBuffer;

    Texture* m_grassTexture;

    /*
      Used for drawing billboards:
     */

    ShaderProgram* m_grassShader;

    void GenerateBillboardVertices(const Vector2f& dir, FloatVector& vertices, UshortVector& indices, const float width, const float height);

public:

    Grass();
    ~Grass();

    void Draw(const Camera& camera);
    void Update(const float delta);
};
