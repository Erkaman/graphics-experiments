#pragma once

#include <string>

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

    const float ComputeY(const unsigned char heightMapData );

public:

    HeightMap(const std::string& path);

    void Draw(const Camera& camera);

    void SetWireframe(const bool wireframe);

};
