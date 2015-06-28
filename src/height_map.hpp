#pragma once

#include <string>

class VBO;

class HeightMap {


private:

    unsigned int m_width;
    unsigned int m_depth;

    std::unique_ptr<VBO> vertexBuffer;
    std::unique_ptr<VBO> indexBuffer;

public:

    HeightMap(const std::string& path);

    void Draw();

};
