#pragma once

#include <string>
#include "math/color.hpp"
#include "math/vector4f.hpp"

class VBO;
class ShaderProgram;
class Camera;
class Texture;

class HeightMap {


private:

    unsigned int m_numTriangles;
    bool m_isWireframe;

    std::unique_ptr<VBO> m_vertexBuffer;
    std::unique_ptr<VBO> m_indexBuffer;

    ShaderProgram* m_shader;

    float m_movement;

    static const float ComputeY(const unsigned char heightMapData );
    static const float ScaleXZ(const int x);
    static const Color VertexColoring(const float y);

    static void CreateHeightmap(const std::string& path);

public:

    HeightMap(const std::string& path);
    ~HeightMap();

    void Draw(const Camera& camera, const Vector4f& lightPosition);

    void SetWireframe(const bool wireframe);



};
