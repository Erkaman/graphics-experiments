#pragma once

#include <string>

class VBO;

class HeightMap {


private:

    unsigned int m_width;
    unsigned int m_depth;

    std::unique_ptr<VBO> vertexBuffer;
    std::unique_ptr<VBO> indexBuffer;

    const float ComputeY(const unsigned char heightMapData );

public:

    HeightMap(const std::string& path);

    void Draw();

};
