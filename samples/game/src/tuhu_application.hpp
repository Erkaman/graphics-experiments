#pragma once

#include "application.hpp"
#include "common.hpp"

class Texture;

class Camera;
class HeightMap;
class Tree;
class FBO;
class Quad;
class PerlinSeed;
class Skydome;

class TuhuApplication : public Application{

private:

    Camera* m_camera;
    HeightMap* m_heightMap;

    FBO* m_fullscreenFbo;

    Quad* m_quad;

    ShaderProgram* m_postShader;

    PerlinSeed* m_perlinSeed;

    Skydome* m_skydome;

public:

    TuhuApplication(int argc, char *argv[]);
    ~TuhuApplication();

    void Init() override;
    void Render() override;
    void RenderText() override;
    void Update(const float delta) override;

};
