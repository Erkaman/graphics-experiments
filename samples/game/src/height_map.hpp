#pragma once

#include <string>
#include "math/color.hpp"
#include "math/vector4f.hpp"
#include "math/vector2f.hpp"

#include "ewa/mult_array.hpp"

class VBO;
class ShaderProgram;
class Camera;
class Texture;
class PerlinSeed;
class Texture;


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

    MultArray<Cell> *m_map;


    static const float ComputeY(const unsigned char heightMapData );
    static const float ScaleXZ(const int x);
    static const Color VertexColoring(const float y);

    static void CreateHeightmap(const std::string& path);

    void Render();

public:

    HeightMap(const std::string& path);
    ~HeightMap();

    void Render(const Camera& camera, const Vector4f& lightPosition);
    void RenderShadowMap(const Camera& camera);

    void SetWireframe(const bool wireframe);

    float GetHeightAt(float x, float z)const;


};
