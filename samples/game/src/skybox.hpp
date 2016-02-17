#pragma once

#include "ewa/gl/cube_map_texture.hpp"

class ShaderProgram;
class VBO;
class ICamera;

class Skybox {

private:


    // skybox shad er.

    VBO* m_indexBuffer;
    VBO* m_positionBuffer;
    ShaderProgram* m_shader;

    GLushort m_numIndices;

public:

    Skybox();
    ~Skybox();

    void Draw(CubeMapTexture* m_cubeMap, const ICamera* camera, Texture* depthMap, int windowWidth, int windowHeight);
};
