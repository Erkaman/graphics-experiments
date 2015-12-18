#pragma once

#include "ewa/gl/gl_common.hpp"

#include "ewa/math/vector2f.hpp"

class VBO;
class ShaderProgram;
class ICamera;
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




    unsigned int m_billboardNumTriangles;
    /*
      These are used to draw the billboard sun:
     */
    VBO* m_billboardVertexBuffer;
    VBO* m_billboardIndexBuffer;



    ShaderProgram* m_grassShader;
    ShaderProgram* m_billboardShader;


    Texture* m_grassTexture;

    float m_time;

    HeightMap* m_heightMap;

    Vector2f m_position;

    void GenerateGrassVertices(const Vector2f position, const float angle, FloatVector& grassVertices, UshortVector& grassIndices, const float width, const float height);


    void GenerateBillboardVertices(const Vector2f position, FloatVector& billboardVertices, UshortVector& billboardIndices, const float width, const float height);


    void MakeGrass(const Vector2f position, const float angle, FloatVector& grassVertices, UshortVector& grassIndices,    FloatVector& billboardVertices, UshortVector& billboardIndices, const float width, const float height);


public:

    Grass(Vector2f position, HeightMap* heightMap);
    ~Grass();

    void Draw(const ICamera* camera, const Vector4f& lightPosition);
    void Update(const float delta);
};
