#include "tuhu_application.hpp"
#include "gl/shader_program.hpp"
#include "gl/vbo.hpp"
#include "gl/vao.hpp"

#include "math/matrix4f.hpp"

#include "gl/texture2d.hpp"

#include "camera.hpp"
#include "height_map.hpp"
#include "tree.hpp"
#include "skybox.hpp"


#include "common.hpp"


using namespace std;

TuhuApplication::TuhuApplication() { }

TuhuApplication::~TuhuApplication() { }

void TuhuApplication::Init() {

    LOG_I("init");

    GL_C(glEnable (GL_DEPTH_TEST)); // enable depth-testing
    GL_C(glEnable (GL_CULL_FACE)); // enable depth-testing



    camera = make_unique<Camera>(GetWindowWidth(),GetWindowHeight(),Vector3f(-1.7f,5.1f,-1.4f), Vector3f(1.0f,-0.5f,1.0f));

    heightMap = make_unique<HeightMap>("img/combined.png");

    tree = make_unique<Tree>(Vector3f(0,0,0));

    skybox = make_unique<Skybox>(
	"img/bluecloud_ft.png",
	"img/bluecloud_bk.png",
	"img/bluecloud_lf.png",
	"img/bluecloud_rt.png",
	"img/bluecloud_up.png",
	"img/bluecloud_dn.png"
	);


}

void TuhuApplication::Render() {
    SetViewport();
    GL_C(glClearColor(0.0f, 0.0f, 1.0f, 1.0f));
    GL_C(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

//    GL_C(glActiveTexture( GL_TEXTURE0));
//    shader->SetUniform("tex", 0);

    skybox->Draw(*camera);

    heightMap->SetWireframe(false);

    Vector4f lightPosition(93,10.0f,93, 1.0f);
    heightMap->Draw(*camera, lightPosition);


    tree->Draw(*camera, lightPosition);

}

void TuhuApplication::Update(const float delta) {
    static float speed = 1.0f;


    if(GetKey( GLFW_KEY_W ) == GLFW_PRESS) {
	camera->Walk(delta * speed);
    }  else if(GetKey( GLFW_KEY_S ) == GLFW_PRESS) {
	camera->Walk(-delta * speed);
    }

    if(GetKey( GLFW_KEY_A ) == GLFW_PRESS) {
	camera->Stride(-delta * speed);
    }else if(GetKey( GLFW_KEY_D ) == GLFW_PRESS) {
	camera->Stride(+delta * speed);
    }

    if(GetKey( GLFW_KEY_O ) == GLFW_PRESS) {
	camera->Fly(+delta * speed);
    }else if(GetKey( GLFW_KEY_L ) == GLFW_PRESS) {
	camera->Fly(-delta * speed);
    }

    if(GetKey( GLFW_KEY_M ) == GLFW_PRESS) {
	speed = 5.0f;
    }else {
	speed = 1.0f;
    }

    camera->HandleInput();


    SetWindowTitle(tos(camera->GetPosition()) );

}

void TextureDeleter::operator()(Texture *p){ delete p;}
