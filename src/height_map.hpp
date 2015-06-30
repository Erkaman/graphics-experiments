#pragma once

#include <string>
#include "math/color.hpp"
#include <memory>
#include "math/vector4f.hpp"

class VBO;
class ShaderProgram;
class Camera;

class HeightMap {


private:

    unsigned int m_width;
    unsigned int m_depth;
    unsigned int m_numTriangles;
    bool m_isWireframe;


    std::unique_ptr<VBO> vertexBuffer;
    std::unique_ptr<VBO> indexBuffer;

    std::unique_ptr<ShaderProgram> shader;

    Vector4f m_lightPosition;
    float m_movement;

    static const float ComputeY(const unsigned char heightMapData );
    static const float ScaleXZ(const float x);
    static const Color VertexColoring(const float y);

public:

    HeightMap(const std::string& path);

    void Draw(const Camera& camera);

    void SetWireframe(const bool wireframe);

};
