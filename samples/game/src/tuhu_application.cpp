#include "tuhu_application.hpp"
#include "gl/shader_program.hpp"
#include "gl/vbo.hpp"
#include "gl/vao.hpp"

#include "math/matrix4f.hpp"

#include "gl/texture2d.hpp"
#include "gl/fbo.hpp"

#include "camera.hpp"
#include "height_map.hpp"
#include "quad.hpp"
#include "perlin_seed.hpp"
#include "keyboard_state.hpp"

#include "worley_seed.hpp"

#include "font.hpp"

#include "common.hpp"

#include "skydome.hpp"

#include "value_noise_seed.hpp"



using namespace std;


TuhuApplication::TuhuApplication(int argc, char *argv[]):Application(argc, argv), m_camera(NULL), m_heightMap(NULL),m_fullscreenFbo(NULL),  m_quad(NULL), m_postShader(NULL), m_perlinSeed(NULL), m_skydome(NULL){ }

TuhuApplication::~TuhuApplication() {
    MY_DELETE(m_camera);
    MY_DELETE(m_heightMap);
    MY_DELETE(m_quad);
    MY_DELETE(m_perlinSeed);
    MY_DELETE(m_skydome);
    MY_DELETE(m_postShader);
    MY_DELETE(m_fullscreenFbo);
}

void TuhuApplication::Init() {

    ::SetDepthTest(true);

    ::SetCullFace(true);

    m_camera = new Camera(GetWindowWidth()*2,GetWindowHeight()*2,Vector3f(0,0.0f,0), Vector3f(1.0f,-0.5f,1.0f), true);

    m_heightMap = new HeightMap("img/combined.png");

	LOG_I("making tree");




    LOG_I("done init");

    m_fullscreenFbo = new FBO(9,
			      GetFramebufferWidth(), GetFramebufferHeight());

    m_quad = new Quad(Vector2f(-1.0f), Vector2f(1.0f));

    m_postShader = new ShaderProgram("shader/post");


    m_perlinSeed = new PerlinSeed(1);

    //                    128000
    m_skydome = new Skydome(1, 10, 10);
}

void TuhuApplication::Render() {

    m_fullscreenFbo->Bind();
    {
	SetViewport();

	Clear(0.0f, 1.0f, 1.0f);

	m_skydome->Draw(*m_camera);



	Vector4f lightPosition(93,10.0f,93, 1.0f);
	m_heightMap->Draw(*m_camera, lightPosition);

    }
    m_fullscreenFbo->Unbind();

    SetViewport();
    Clear(1.0f, 1.0f, 1.0f);


    m_postShader->Bind();


    Texture::SetActiveTextureUnit(m_fullscreenFbo->GetTargetTextureUnit());

    m_postShader->SetUniform("tex", (int)m_fullscreenFbo->GetTargetTextureUnit() );

    m_fullscreenFbo->GetRenderTargetTexture().Bind();

    m_quad->Draw();


    m_fullscreenFbo->GetRenderTargetTexture().Unbind();


    m_postShader->Unbind();

}

void TuhuApplication::Update(const float delta) {
    m_camera->HandleInput(delta);

    m_skydome->Update(delta);
}

void TuhuApplication::RenderText()  {
    m_font->DrawString(*m_fontShader, 600,150, "hello world" );
}
