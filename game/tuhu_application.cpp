#include "tuhu_application.hpp"
#include "gl/shader_program.hpp"
#include "gl/vbo.hpp"
#include "gl/vao.hpp"

#include "math/matrix4f.hpp"

#include "gl/texture2d.hpp"

#include "camera.hpp"
#include "height_map.hpp"

using namespace std;

TuhuApplication::TuhuApplication() { }

TuhuApplication::~TuhuApplication() { }

void TuhuApplication::Init() {

    VAO().Bind();
    LOG_I("init");

    GL_C(glEnable (GL_DEPTH_TEST)); // enable depth-testing
    GL_C(glEnable (GL_CULL_FACE)); // enable depth-testing

/*    texture->Bind();
    texture->SetTextureTiling();
    texture->SetMinFilter(GL_LINEAR);
    texture->SetMagFilter(GL_NEAREST);
    texture->Unbind();
*/
    camera = make_unique<Camera>(GetWindowWidth(),GetWindowHeight(),Vector3f(-0.5,0.6,-0.5), Vector3f(1,-0.5,1));

    heightMap = make_unique<HeightMap>("img/cloud.png");

}

void TuhuApplication::Render() {
    SetViewport();
    GL_C(glClearColor(0.0f, 0.0f, 1.0f, 1.0f));
    GL_C(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

//    GL_C(glActiveTexture( GL_TEXTURE0));
//    shader->SetUniform("tex", 0);

    heightMap->SetWireframe(false);

    heightMap->Draw(*camera);
}

void TuhuApplication::Update(const double delta) {
    if(GetKey( GLFW_KEY_W ) == GLFW_PRESS) {
	camera->Walk(delta);
    }  else if(GetKey( GLFW_KEY_S ) == GLFW_PRESS) {
	camera->Walk(-delta);
    }

    if(GetKey( GLFW_KEY_A ) == GLFW_PRESS) {
	camera->Stride(-delta);
    }else if(GetKey( GLFW_KEY_D ) == GLFW_PRESS) {
	camera->Stride(+delta);
    }

    if(GetKey( GLFW_KEY_O ) == GLFW_PRESS) {
	camera->Fly(+delta);
    }else if(GetKey( GLFW_KEY_L ) == GLFW_PRESS) {
	camera->Fly(-delta);
    }

    camera->HandleInput();
}

void TextureDeleter::operator()(Texture *p){ delete p;}
