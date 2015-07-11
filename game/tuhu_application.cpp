#include "tuhu_application.hpp"
#include "gl/shader_program.hpp"
#include "gl/vbo.hpp"
#include "gl/vao.hpp"

#include "math/matrix4f.hpp"

#include "gl/texture2d.hpp"
#include "gl/fbo.hpp"

#include "camera.hpp"
#include "height_map.hpp"
#include "tree.hpp"
#include "skybox.hpp"
#include "plane.hpp"
#include "quad.hpp"
#include "perlin_seed.hpp"

#include "font.hpp"


#include "common.hpp"


using namespace std;

constexpr int TEXTURE_SIZE = 512;

TuhuApplication::TuhuApplication(): camera(NULL), heightMap(NULL), skybox(NULL), tree(NULL), plane(NULL), fbo(NULL), quad(NULL), simpleShader(NULL), m_perlinSeed(NULL) { }

TuhuApplication::~TuhuApplication() {
    MY_DELETE(camera);
    MY_DELETE(heightMap);
    MY_DELETE(skybox);
    MY_DELETE(tree);
    MY_DELETE(plane);
    MY_DELETE(fbo);
    MY_DELETE(quad);
    MY_DELETE(simpleShader);
    MY_DELETE(m_perlinSeed);
}

void TuhuApplication::Init() {

    LOG_I("init");

    GL_C(glEnable (GL_DEPTH_TEST)); // enable depth-testing
    GL_C(glEnable (GL_CULL_FACE)); // enable depth-testing

	LOG_I("making camera");

	camera = new Camera(GetWindowWidth(),GetWindowHeight(),Vector3f(0,5.1f,0), Vector3f(1.0f,-0.5f,1.0f), true);

	LOG_I("making height map");


    heightMap = new HeightMap("img/combined.png");

/*	LOG_I("making tree");

    tree = make_unique<Tree>(Vector3f(0,2,0));
*/

    skybox = new Skybox(
	"img/bluecloud_ft.png",
	"img/bluecloud_bk.png",
	"img/bluecloud_lf.png",
	"img/bluecloud_rt.png",
	"img/bluecloud_up.png",
	"img/bluecloud_dn.png"
	);



//    plane = new Plane(Vector3f(1,4,1));

    LOG_I("done init");

    fbo = new FBO(10, TEXTURE_SIZE,TEXTURE_SIZE);

    quad = new Quad(Vector2f(0.0f), Vector2f(TEXTURE_SIZE / 256.0f));

    simpleShader = new ShaderProgram("shader/simple");

    m_perlinSeed = new PerlinSeed(1);

}

void TuhuApplication::Render() {

    SetViewport();

    Clear(0.0f, 0.0f, 1.0f);

     skybox->Draw(*camera);

    heightMap->SetWireframe(false);

    Vector4f lightPosition(93,10.0f,93, 1.0f);
    heightMap->Draw(*camera, lightPosition);



//fbo->Bind();
    {
	::SetViewport(-256,-256,512,512);
	Clear(0.0f, 0.0f, 1.0f);

    	simpleShader->Bind();

	m_perlinSeed->Bind(*simpleShader);

	quad->Draw();

	m_perlinSeed->Unbind();

	simpleShader->Unbind();

    }
    /* fbo->Unbind();
         fbo->GetRenderTargetTexture().WriteToFile("out.png");
            exit(1);
    */


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
}

void TuhuApplication::RenderText()  {

/*

    GL_C(glViewport(0, 0, 100, 100));
    GL_C(glClearColor(0.0f, 0.0f, 1.0f, 1.0f));
    GL_C(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

    fbo->Unbind();

    fbo->GetRenderTargetTexture().WriteToFile("out.png");
    exit(2);*/

    m_font->DrawString(*m_fontShader, 600,150, "hello world" );

}
