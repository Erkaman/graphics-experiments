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
    ShaderProgram* m_deferredShader;
    ShaderProgram* m_forwardShader;

    GLushort m_numIndices;

public:

    Skybox();
    ~Skybox();

    void DrawDeferred(CubeMapTexture* m_cubeMap, const ICamera* camera, Texture* depthMap, int windowWidth, int windowHeight);

    void DrawForward(CubeMapTexture* m_cubeMap, const ICamera* camera);
};
