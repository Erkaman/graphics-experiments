#pragma once

#include "application.hpp"
#include "gl/vbo.hpp"

class ShaderProgram;
struct ShaderProgramDeleter
{
    void operator()(ShaderProgram *p);
    };

class TuhuApplication : public Application{

private:

    std::unique_ptr<ShaderProgram> shader;
//    VBO positionsBuffer;

public:

    TuhuApplication();
    ~TuhuApplication();

    void Init() override;
    void Render() override;
    void Update() override;


};
