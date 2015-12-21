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
class ShaderProgra;
class Config;
class ICamera;

struct Cell {
    Vector3f position;
    float id;
    Vector2f texCoord;
};

class HeightMap {


private:

    unsigned int m_numTriangles;
    bool m_isWireframe;

    VBO* m_vertexBuffer;
    VBO* m_indexBuffer;

    VBO* m_cursorVertexBuffer;
    unsigned short m_numCursorPoints;

    ShaderProgram* m_shader;
    ShaderProgram* m_depthShader; //outputs only the depth. Used for shadow mapping.
    ShaderProgram* m_idShader; //outputs only the id. Used for triangle picking in the height map.
    ShaderProgram* m_cursorShader;

    Texture* m_grassTexture;
    Texture* m_sandTexture;
    Texture* m_snowTexture;
    Texture2D* m_imageTexture;

    MultArray<Cell> *m_map;
    MultArray<unsigned short>* m_image;

    Config& m_config;

    Vector3f cursorPosition;

    static const float ComputeY(const unsigned char heightMapData );
    static const float ScaleXZ(const int x);
    static const Color VertexColoring(const float y);

    void CreateHeightmap(const std::string& path);
    void CreateCursor();

    void RenderHeightMap(const ICamera* camera, const Vector4f& lightPosition);
    void RenderCursor(const ICamera* camera);


    void Render(ShaderProgram* shader);

    void RenderSetup(ShaderProgram* shader);
    void RenderUnsetup(ShaderProgram* shader);

    void UpdateCursor(ICamera* camera,
		      const float framebufferWidth,
		      const float framebufferHeight);

public:

    HeightMap(const std::string& path);
    ~HeightMap();

    void Render(const ICamera* camera, const Vector4f& lightPosition);
    void RenderShadowMap(const ICamera* camera);

    // render, but instead of outputting colors for every triangle, we output the id of the frontmost triangles.
    void RenderId(const ICamera* camera);

    void SetWireframe(const bool wireframe);

    float GetHeightAt(float x, float z)const;

    void Update(const float delta, ICamera* camera,
		const float framebufferWidth,
		const float framebufferHeight);


    void ModifyTerrain(const float delta);

};
