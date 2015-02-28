#include "tuhu_application.hpp"
#include "gl/shader/shader_program.hpp"
#include "gl/vbo.hpp"
#include "gl/vao.hpp"

#include "fileutil.hpp"

using namespace std;


TuhuApplication::TuhuApplication() {
    shader = nullptr;
}

TuhuApplication::~TuhuApplication() {
}

void TuhuApplication::Init() {

    VAO().Bind();

    shader = make_unique<ShaderProgram>("shader/simple");

//    positionsBuffer = createPositionVBO(3);

    GL_C(glEnable (GL_DEPTH_TEST)); // enable depth-testing
    GL_C(glDisable(GL_CULL_FACE));
}

void TuhuApplication::Render() {

    SetViewport();
    GL_C(glClearColor(0.0f, 0.0f, 1.0f, 1.0f));
    GL_C(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}

void TuhuApplication::Update() {
}

void ShaderProgramDeleter::operator()(ShaderProgram *p)
{
    delete p;
}
