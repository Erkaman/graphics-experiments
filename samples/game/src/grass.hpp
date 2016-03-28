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

    VBO* m_grassVertexBuffer;
    VBO* m_grassIndexBuffer;




    ShaderProgram* m_deferredShader;
    ShaderProgram* m_reflectionShader;


    Texture* m_grassTexture;

    float m_time;

    HeightMap* m_heightMap;

    Vector2f m_position;

    void GenerateGrassVertices(const Vector2f position, const float angle, FloatVector& grassVertices, UshortVector& grassIndices, const float width, const float height);


    void MakeGrass(const Vector2f position, const float angle, FloatVector& grassVertices, UshortVector& grassIndices,    FloatVector& billboardVertices, UshortVector& billboardIndices, const float width, const float height);

    void Draw(const ICamera* camera, const Vector4f& lightPosition, ShaderProgram* shader);

public:

    Grass(Vector2f position, HeightMap* heightMap);
    ~Grass();

    void DrawDeferred(const ICamera* camera, const Vector4f& lightPosition);
    void DrawReflection(const ICamera* camera, const Vector4f& lightPosition);

    void Update(const float delta);


};
