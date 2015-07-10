#pragma once

#include "application.hpp"
#include "common.hpp"

class Texture;
struct TextureDeleter{void operator()(Texture *p);};

class Camera;
class HeightMap;
class Skybox;
class Tree;
class Plane;
class FBO;

class TuhuApplication : public Application{

private:

    Camera* camera;
    HeightMap* heightMap;
    Skybox* skybox;
    Tree* tree;
    Plane* plane;
    FBO* fbo;

public:

    TuhuApplication();
    ~TuhuApplication();

    void Init() override;
    void Render() override;
    void RenderText() override;
    void Update(const float delta) override;

};
