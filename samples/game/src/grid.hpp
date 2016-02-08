#pragma once

#include "ewa/gl/cube_map_texture.hpp"

class ShaderProgram;
class VBO;
class Texture;

class Grid {

private:


    VBO* m_positionBuffer;
    ShaderProgram* m_shader;

    int m_numTris;

    Texture* m_gridTexture;




public:

    Grid();
    ~Grid() {}

    void Draw();
};
