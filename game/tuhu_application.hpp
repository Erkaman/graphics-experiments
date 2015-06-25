#pragma once

#include "application.hpp"
#include "common.hpp"

class ShaderProgram;
struct ShaderProgramDeleter{void operator()(ShaderProgram *p);};

class VBO;
struct VBODeleter{void operator()(VBO *p);};

class Texture;
struct TextureDeleter{void operator()(Texture *p);};

class TuhuApplication : public Application{

private:

    std::unique_ptr<ShaderProgram> shader;
    std::unique_ptr<VBO> positionsBuffer;
    std::unique_ptr<VBO> texCoordsBuffer;
    std::unique_ptr<Texture> texture;

public:

    TuhuApplication();
    ~TuhuApplication();

    void Init() override;
    void Render() override;
    void Update() override;
};
