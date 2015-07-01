#pragma once

#include "gl/cube_map_texture.hpp"

#include <memory>

class ShaderProgram;
struct ShaderProgramDeleter{void operator()(ShaderProgram *p);};

class VBO;
class Camera;

class Skybox {

private:

    CubeMapTexture m_cubeMap;

    // skybox shad er.

    std::unique_ptr<VBO> m_indexBuffer;
    std::unique_ptr<VBO> m_positionBuffer;
    std::unique_ptr<ShaderProgram> m_shader;

    GLushort m_numIndices;


    void SetupForRender();
    void UnsetupForRender();

public:

    Skybox(const std::string& frontFace, const std::string& backFace, const std::string& leftFace,
		   const std::string& rightFace, const std::string& upFace, const std::string& downFace);

    void Draw(const Camera& camera);

};
