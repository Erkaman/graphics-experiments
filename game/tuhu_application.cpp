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

    vector<Vector3f> vertices;

    vertices.emplace_back(-0.5f,0.5f,0.0f); // top left
    vertices.emplace_back(-0.5f,-0.5f,0.0f); // bottom left
    vertices.emplace_back(0.5f,-0.5f,0.0f); // bottom right
    vertices.emplace_back(0.5f,0.5f,0.0f); // top right

    positionsBuffer->Bind();
    positionsBuffer->SetBufferData(vertices);
    positionsBuffer->Unbind();

    vector<Vector2f> texCoords;

    texCoords.emplace_back(0,0);
    texCoords.emplace_back(0,1);
    texCoords.emplace_back(1,1);
    texCoords.emplace_back(1,0);

    texCoordsBuffer->Bind();
    texCoordsBuffer->SetBufferData(texCoords);
    texCoordsBuffer->Unbind();

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
    texture->SetTextureClamping();
    texture->SetMinFilter(GL_LINEAR);
    texture->SetMagFilter(GL_NEAREST);
    texture->Unbind();
}

void TuhuApplication::Render() {
    SetViewport();
    GL_C(glClearColor(0.0f, 0.0f, 1.0f, 1.0f));
    GL_C(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

    shader->Bind();

    Matrix4f view = Matrix4f::CreateLookAt(
	Vector3f(0,0,1), // eye
	Vector3f(0,0,0), // center
	Vector3f(0,1,0) // up
	);
    shader->SetUniform("view", view);

    Matrix4f proj = Matrix4f::CreatePerspective(90.0f, (float)GetWindowWidth()/(float)GetWindowHeight(), 0.01f,100.0f);
    shader->SetUniform("proj", proj);


    positionsBuffer->Bind();
    positionsBuffer->EnableVertexAttrib();
    positionsBuffer->Unbind();

    texCoordsBuffer->Bind();
    texCoordsBuffer->EnableVertexAttrib();
    texCoordsBuffer->Unbind();



    texture->Bind();
    GL_C(glActiveTexture( GL_TEXTURE0));
    shader->SetUniform("tex", 0);

    indexBuffer->Bind();

    indexBuffer->DrawIndices(GL_TRIANGLES, 6);

    indexBuffer->Unbind();


    texture->Unbind();


    texCoordsBuffer->Bind();
    texCoordsBuffer->DisableVertexAttrib();
    texCoordsBuffer->Unbind();

    positionsBuffer->Bind();
    positionsBuffer->DisableVertexAttrib();
    positionsBuffer->Unbind();


    shader->Unbind();
}

void TuhuApplication::Update() {}

void ShaderProgramDeleter::operator()(ShaderProgram *p){ delete p;}

void VBODeleter::operator()(VBO *p){ delete p;}

void TextureDeleter::operator()(Texture *p){ delete p;}
