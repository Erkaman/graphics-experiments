#pragma once

#include "application.h"
#include "gl/vbo.h"

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
