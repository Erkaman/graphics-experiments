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

    std::unique_ptr<Camera> camera;
    std::unique_ptr<HeightMap> heightMap;
    std::unique_ptr<Skybox> skybox;
    std::unique_ptr<Tree> tree;
    std::unique_ptr<Plane> plane;
    std::unique_ptr<FBO> fbo;



public:

    TuhuApplication();
    ~TuhuApplication();

    void Init() override;
    void Render() override;
    void RenderText() override;
    void Update(const float delta) override;

};
