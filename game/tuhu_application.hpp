#pragma once

#include "application.hpp"
#include "common.hpp"

class Texture;

class Camera;
class HeightMap;
class Skybox;
class Tree;
class Plane;
class FBO;
class Quad;
class PerlinSeed;

class TuhuApplication : public Application{

private:

    Camera* camera;
    HeightMap* heightMap;
    Skybox* skybox;
    Tree* tree;
    Plane* plane;

    FBO* fbo;

    Quad* quad;
    ShaderProgram* simpleShader;

    PerlinSeed* m_perlinSeed;


public:

    TuhuApplication();
    ~TuhuApplication();

    void Init() override;
    void Render() override;
    void RenderText() override;
    void Update(const float delta) override;

};
