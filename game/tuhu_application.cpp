#include "tuhu_application.hpp"
#include "gl/shader_program.hpp"
#include "gl/vbo.hpp"
#include "gl/vao.hpp"
#include "gl/vbo.hpp"

#include "file.hpp"

#include "math/vector3f.hpp"
#include "math/color.hpp"
#include "math/matrix4f.hpp"


using namespace std;


TuhuApplication::TuhuApplication() {
    shader = nullptr;
}

TuhuApplication::~TuhuApplication() {
}

void TuhuApplication::Init() {

    VAO().Bind();
    LOG_I("init");

    shader = make_unique<ShaderProgram>("shader/simple");

    positionsBuffer = unique_ptr<VBO>(createPositionVBO(3));

    vector<Vector3f> vertices;

    vertices.push_back(Vector3f(0.0f,0.5f,0.0f));
    vertices.push_back(Vector3f(-0.5f,-0.5f,0.0f));
    vertices.push_back(Vector3f(0.5f,-0.5f,0.0f));

    positionsBuffer->Bind();
    positionsBuffer->SetBufferData(vertices.size()*4*3, &vertices[0]);
    positionsBuffer->Unbind();

    GL_C(glEnable (GL_DEPTH_TEST)); // enable depth-testing
    GL_C(glDisable(GL_CULL_FACE));


    /*
    Matrix4f m;
    m.setIdentity();

    */
//    LOG_I("mat:\n%s", string(m).c_str());
}

void TuhuApplication::Render() {
    SetViewport();
    GL_C(glClearColor(0.0f, 0.0f, 1.0f, 1.0f));
    GL_C(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));


    shader->Bind();

    positionsBuffer->Bind();
    positionsBuffer->EnableVertexAttrib();
    positionsBuffer->Unbind();

    Color c(1.0f, 0.0f, 0.0f);
    shader->SetUniform("color", c);

    Matrix4f m = Matrix4f::CreateTranslation(0.0f,-0.4,0);
    shader->SetUniform("m", m);

//    Matrix4f pers = Matrix4f::CreatePerspective(0,0,0,0);

    GL_C(glDrawArrays( GL_TRIANGLES, 0, 3 ));

    positionsBuffer->Bind();
    positionsBuffer->DisableVertexAttrib();
    positionsBuffer->Unbind();

    shader->Unbind();
}

void TuhuApplication::Update() {}

void ShaderProgramDeleter::operator()(ShaderProgram *p){ delete p;}

void VBODeleter::operator()(VBO *p){ delete p;}
