#include "tuhu_application.hpp"
#include "gl/shader_program.hpp"
#include "gl/vbo.hpp"
#include "gl/vao.hpp"
#include "gl/vbo.hpp"

#include "file.hpp"

#include "math/vector3f.hpp"
#include "math/vector2f.hpp"
#include "math/color.hpp"
#include "math/matrix4f.hpp"

#include "gl/texture2d.hpp"

#include "camera.hpp"

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

    positionsBuffer = unique_ptr<VBO>(VBO::CreatePosition(3));
    texCoordsBuffer = unique_ptr<VBO>(VBO::CreateTexCoord(2));
    indexBuffer = unique_ptr<VBO>(VBO::CreateIndex());

    vertexBuffer = unique_ptr<VBO>(VBO::CreateInterleaved(
				       vector<GLuint>{VBO_POSITION_ATTRIB_INDEX, VBO_TEX_COORD_ATTRIB_INDEX},
				       vector<GLuint>{3,2}
				       ));

    vector<GLfloat> vertices;

    vertices.push_back(-0.5f); vertices.push_back(+10.5f); vertices.push_back(0.0f);
    vertices.push_back(0); vertices.push_back(-10);

    vertices.push_back(-0.5f); vertices.push_back(-0.5f); vertices.push_back(0.0f);
    vertices.push_back(0); vertices.push_back(1);

    vertices.push_back(+2.5f); vertices.push_back(-0.5f); vertices.push_back(0.0f);
    vertices.push_back(3); vertices.push_back(1);

    vertices.push_back(+2.5f); vertices.push_back(+10.5f); vertices.push_back(0.0f);
    vertices.push_back(3); vertices.push_back(-10);

    vertexBuffer->Bind();
    vertexBuffer->SetBufferData(vertices);
    vertexBuffer->Unbind();

    vector<GLushort> indices;
    indices.emplace_back(0);
    indices.emplace_back(1);
    indices.emplace_back(2);

    indices.emplace_back(2);
    indices.emplace_back(3);
    indices.emplace_back(0);

    indexBuffer->Bind();
    indexBuffer->SetBufferData(indices);
    indexBuffer->Unbind();


    GL_C(glEnable (GL_DEPTH_TEST)); // enable depth-testing

    texture = unique_ptr<Texture>(new Texture2D ("img/floor.png"));

    texture->Bind();
    texture->SetTextureTiling();
    texture->SetMinFilter(GL_LINEAR);
    texture->SetMagFilter(GL_NEAREST);
    texture->Unbind();

    camera = make_unique<Camera>(GetWindowWidth(),GetWindowHeight(),Vector3f(0,-0.4,2.9), Vector3f(0,2.3,-2.9));
}

void TuhuApplication::Render() {
    SetViewport();
    GL_C(glClearColor(0.0f, 0.0f, 1.0f, 1.0f));
    GL_C(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

    shader->Bind();

    shader->SetUniform("mvp", camera->GetMvp());

    vertexBuffer->EnableVertexAttribInterleaved();

    texture->Bind();
    GL_C(glActiveTexture( GL_TEXTURE0));
    shader->SetUniform("tex", 0);

    indexBuffer->Bind();

    indexBuffer->DrawIndices(GL_TRIANGLES, 6);

    indexBuffer->Unbind();


    texture->Unbind();

    vertexBuffer->DisableVertexAttribInterleaved();


    shader->Unbind();
}

void TuhuApplication::Update(const double delta) {
    if(GetKey( GLFW_KEY_W ) == GLFW_PRESS) {
	camera->Walk(delta);
    }  else if(GetKey( GLFW_KEY_S ) == GLFW_PRESS) {
	camera->Walk(-delta);
    } else if(GetKey( GLFW_KEY_A ) == GLFW_PRESS) {
	camera->Stride(-delta);
    }else if(GetKey( GLFW_KEY_D ) == GLFW_PRESS) {
	camera->Stride(+delta);
    }

    camera->HandleInput();
}

void ShaderProgramDeleter::operator()(ShaderProgram *p){ delete p;}

void VBODeleter::operator()(VBO *p){ delete p;}

void TextureDeleter::operator()(Texture *p){ delete p;}
