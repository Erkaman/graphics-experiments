#pragma once

#include "gl/cube_map_texture.hpp"

class ShaderProgram;
class VBO;
class Camera;
class PerlinSeed;

class Skybox {

private:

    CubeMapTexture m_cubeMap;

    // skybox shad er.

    VBO* m_indexBuffer;
    VBO* m_positionBuffer;
    ShaderProgram* m_shader;

    GLushort m_numIndices;


    PerlinSeed* m_perlinSeed;

    void SetupForRender();
    void UnsetupForRender();

public:

    Skybox(const std::string& frontFace, const std::string& backFace, const std::string& leftFace,
		   const std::string& rightFace, const std::string& upFace, const std::string& downFace);
    ~Skybox();

    void Draw(const Camera& camera);

};
