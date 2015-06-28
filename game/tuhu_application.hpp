#pragma once

#include "application.hpp"
#include "common.hpp"

class ShaderProgram;
struct ShaderProgramDeleter{void operator()(ShaderProgram *p);};

class VBO;
struct VBODeleter{void operator()(VBO *p);};

class Texture;
struct TextureDeleter{void operator()(Texture *p);};

class Camera;
class HeightMap;

class TuhuApplication : public Application{

private:

    std::unique_ptr<ShaderProgram> shader;

    std::unique_ptr<Camera> camera;
    std::unique_ptr<HeightMap> heightMap;


public:

    TuhuApplication();
    ~TuhuApplication();

    void Init() override;
    void Render() override;
    void Update(const double delta) override;
};
