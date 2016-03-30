#pragma once

#include "ewa/gl/gl_common.hpp"

#include "ewa/math/vector2f.hpp"


#include "ewa/random.hpp"

#include <map>

class VBO;
class ShaderProgram;
class ICamera;
class Texture;
class Vector2f;
class Vector3f;
class Vector4f;
class HeightMap;
class Vector2i;


struct GrassInfo {
public:
    Vector2f pos;
    float angle;
    float size;
};

class Grass{

private:

    int m_currentId;

    Random m_rng;


    std::map<int,GrassInfo> m_grass;



    unsigned int m_grassNumTriangles;

    VBO* m_grassVertexBuffer;
    VBO* m_grassIndexBuffer;




    ShaderProgram* m_deferredShader;
    ShaderProgram* m_reflectionShader;
    ShaderProgram* m_outputIdShader;


    Texture* m_grassTexture;

    float m_time;

    HeightMap* m_heightMap;

    void GenerateGrassVertices(const Vector2f position, const float angle, FloatVector& grassVertices, UshortVector& grassIndices, const float width, const float height, int id);


    void MakeGrass(const Vector2f position, const float angle, FloatVector& grassVertices, UshortVector& grassIndices,    FloatVector& billboardVertices, UshortVector& billboardIndices, const float width, const float height, int id);

    void Draw(const ICamera* camera, const Vector4f& lightPosition, ShaderProgram* shader);

    void Rebuild();

    void Init();


public:

    Grass(const std::string& filename, HeightMap* heightMap);
    Grass(HeightMap* heightMap);

    ~Grass();

    void DrawDeferred(const ICamera* camera, const Vector4f& lightPosition);
    void DrawReflection(const ICamera* camera, const Vector4f& lightPosition);

    void RenderIdAll(const ICamera* camera);


    void Update(const float delta);

    void AddGrass(const Vector2i& position, int grassClusterSize);
    void RemoveGrass(int id);

    void SaveGrass(const std::string& filename);

};
