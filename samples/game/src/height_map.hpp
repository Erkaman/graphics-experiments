#pragma once

#include <string>
#include "math/color.hpp"
#include "math/vector4f.hpp"
#include "math/vector2f.hpp"

#include "ewa/mult_array.hpp"

class VBO;
class ShaderProgram;
class ICamera;
class Texture;
class PerlinSeed;
class Texture;
class Texture2D;

struct Cell {
    Vector3f position;
    Vector3f normal;
    Vector2f texCoord;
};

class HeightMap {


private:

    unsigned int m_numTriangles;
    bool m_isWireframe;

    VBO* m_vertexBuffer;
    VBO* m_indexBuffer;

    ShaderProgram* m_shader;
    ShaderProgram* m_depthShader; //outputs only the depth. Used for shadow mapping.

    float m_movement;

    Texture* m_grassTexture;
    Texture* m_sandTexture;
    Texture* m_snowTexture;
//    Texture* m_heightMap;

    Texture2D* m_imageTexture;

    MultArray<Cell> *m_map;

    MultArray<unsigned char>* m_image;


    static const float ComputeY(const unsigned char heightMapData );
    static const float ScaleXZ(const int x);
    static const Color VertexColoring(const float y);

    void CreateHeightmap(const std::string& path);

    void Render();

public:

    HeightMap(const std::string& path);
    ~HeightMap();

    void Render(const ICamera* camera, const Vector4f& lightPosition);
    void RenderShadowMap(const ICamera* camera);

    void SetWireframe(const bool wireframe);

    float GetHeightAt(float x, float z)const;

    void Update(const float delta);

};
