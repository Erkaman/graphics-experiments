#pragma once

#include "application.hpp"

class ShaderProgram;
struct ShaderProgramDeleter{void operator()(ShaderProgram *p);};

class VBO;
struct VBODeleter{void operator()(VBO *p);};

class TuhuApplication : public Application{

private:

    std::unique_ptr<ShaderProgram> shader;
    std::unique_ptr<VBO> positionsBuffer;

public:

    TuhuApplication();
    ~TuhuApplication();

    void Init() override;
    void Render() override;
    void Update() override;


};
