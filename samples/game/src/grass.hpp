#pragma once

#include "ewa/gl/gl_common.hpp"


class VBO;
class ShaderProgram;
class Camera;
class Texture;
class Vector2f;
class Vector3f;
class Vector4f;
class HeightMap;

class Grass{

private:

    unsigned int m_grassNumTriangles;
    /*
      These are used to draw the billboard sun:
     */
    VBO* m_grassVertexBuffer;
    VBO* m_grassIndexBuffer;

    Texture* m_grassTexture;

    float m_time;

    /*
      Used for drawing billboards:
     */

    ShaderProgram* m_grassShader;

    HeightMap* m_heightMap;

    void GenerateBillboardVertices(const Vector2f position, const float angle, FloatVector& vertices, UshortVector& indices, const float width, const float height);

    void MakeGrass(const Vector2f position, const float angle, FloatVector& vertices, UshortVector& indices, const float width, const float height);


public:

    Grass(HeightMap* heightMap);
    ~Grass();

    void Draw(const Camera& camera, const Vector4f& lightPosition);
    void Update(const float delta);
};
