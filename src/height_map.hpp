#pragma once

#include <string>

class VBO;
class ShaderProgram;
class Matrix4f;

class HeightMap {


private:

    unsigned int m_width;
    unsigned int m_depth;
    unsigned int m_numTriangles;
    bool m_isWireframe;


    std::unique_ptr<VBO> vertexBuffer;
    std::unique_ptr<VBO> indexBuffer;

    std::unique_ptr<ShaderProgram> shader;

    const float ComputeY(const unsigned char heightMapData );

public:

    HeightMap(const std::string& path);

    void Draw(const Matrix4f& mvp);

    void SetWireframe(const bool wireframe);

};
