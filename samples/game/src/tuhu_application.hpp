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
class Sphere;

class TuhuApplication : public Application{

private:

    Camera* camera;
    HeightMap* heightMap;
    Skybox* skybox;
    Tree* tree;
    Plane* plane;

    FBO* fbo;
    FBO* m_fullscreenFbo;

    Quad* quad;

    ShaderProgram* simpleShader;
    ShaderProgram* m_postShader;

    PerlinSeed* m_perlinSeed;

    Sphere* m_sphere;


public:

    TuhuApplication(int argc, char *argv[]);
    ~TuhuApplication();

    void Init() override;
    void Render() override;
    void RenderText() override;
    void Update(const float delta) override;

};
