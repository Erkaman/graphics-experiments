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
#include "quad.hpp"
#include "perlin_seed.hpp"
#include "keyboard_state.hpp"

#include "worley_seed.hpp"

#include "font.hpp"


#include "common.hpp"

#include "sphere.hpp"

#include "value_noise_seed.hpp"



using namespace std;

constexpr int TEXTURE_SIZE = 256;

TuhuApplication::TuhuApplication(int argc, char *argv[]):Application(argc, argv), camera(NULL), heightMap(NULL),tree(NULL),fbo(NULL),m_fullscreenFbo(NULL),  quad(NULL), simpleShader(NULL), m_postShader(NULL), m_perlinSeed(NULL), m_sphere(NULL){ }

TuhuApplication::~TuhuApplication() {
    MY_DELETE(camera);
    MY_DELETE(heightMap);
    MY_DELETE(tree);
    MY_DELETE(fbo);
    MY_DELETE(quad);
    MY_DELETE(simpleShader);
    MY_DELETE(m_perlinSeed);
    MY_DELETE(m_sphere);
    MY_DELETE(m_postShader);
    MY_DELETE(m_fullscreenFbo);
}

void TuhuApplication::Init() {

    ::SetDepthTest(true);

    ::SetCullFace(true);

    camera = new Camera(GetWindowWidth()*2,GetWindowHeight()*2,Vector3f(0,0.0f,0), Vector3f(1.0f,-0.5f,1.0f), true);

    heightMap = new HeightMap("img/combined.png");

/*	LOG_I("making tree");

    tree = make_unique<Tree>(Vector3f(0,2,0));
*/


    LOG_I("done init");

    fbo = new FBO(10, TEXTURE_SIZE,TEXTURE_SIZE);
    m_fullscreenFbo = new FBO(9,
			      GetFramebufferWidth(), GetFramebufferHeight());

    quad = new Quad(Vector2f(-1.0f), Vector2f(1.0f));

    simpleShader = new ShaderProgram("shader/simple");

    m_postShader = new ShaderProgram("shader/post");


    m_perlinSeed = new PerlinSeed(1);

    //                    128000
    m_sphere = new Sphere(1, 10, 10);
}

void TuhuApplication::Render() {

//    m_fullscreenFbo->Bind();
    {
	SetViewport();

	Clear(0.0f, 1.0f, 1.0f);

	m_sphere->Draw(*camera);

	Vector4f lightPosition(93,10.0f,93, 1.0f);
	heightMap->Draw(*camera, lightPosition);

    }
//    m_fullscreenFbo->Unbind();

//     m_fullscreenFbo->GetRenderTargetTexture().WriteToFile("out.png");
//     exit(1);

/*    SetViewport();
    Clear(1.0f, 1.0f, 1.0f);


    m_postShader->Bind();


    Texture::SetActiveTextureUnit(m_fullscreenFbo->GetTargetTextureUnit());

    m_postShader->SetUniform("tex", (int)m_fullscreenFbo->GetTargetTextureUnit() );

    m_fullscreenFbo->GetRenderTargetTexture().Bind();


    quad->Draw();


    m_fullscreenFbo->GetRenderTargetTexture().Unbind();


    m_postShader->Unbind();
*/

    //fbo = new FBO(10, TEXTURE_SIZE,TEXTURE_SIZE);

    //quad = new Quad(Vector2f(-1.0f), Vector2f(1.0f));

/*
    fbo->Bind();

    ::SetViewport(0,0,TEXTURE_SIZE,TEXTURE_SIZE);

    Clear(0.0f, 0.0f, 1.0f);

    simpleShader->Bind();

    m_perlinSeed->Bind(*simpleShader);

    quad->Draw();

    m_perlinSeed->Unbind();

    simpleShader->Unbind();


    fbo->Unbind();
    fbo->GetRenderTargetTexture().WriteToFile("out.png");
    exit(1);*/
}

void TuhuApplication::Update(const float delta) {
    camera->HandleInput(delta);

    m_sphere->Update(delta);
}

void TuhuApplication::RenderText()  {
    m_font->DrawString(*m_fontShader, 600,150, "hello world" );
}
