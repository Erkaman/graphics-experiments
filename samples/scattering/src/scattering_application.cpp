#include "scattering_application.hpp"

#include "gl/shader_program.hpp"
#include "gl/fbo.hpp"
#include "gl/color_fbo.hpp"
#include "gl/texture.hpp"

#include "ewa/camera.hpp"
#include "ewa/quad.hpp"

#include "skydome.hpp"

#include "ewa/math/vector2f.hpp"
#include "ewa/math/vector4f.hpp"




using namespace std;


ScatteringApplication::ScatteringApplication(int argc, char *argv[]):Application(argc, argv), m_camera(NULL), m_fullscreenFbo(NULL),  m_quad(NULL), m_postShader(NULL), m_skydome(NULL){ }

ScatteringApplication::~ScatteringApplication() {
    MY_DELETE(m_camera);
    MY_DELETE(m_quad);
    MY_DELETE(m_skydome);
    MY_DELETE(m_postShader);
    MY_DELETE(m_fullscreenFbo);
}

void ScatteringApplication::Init() {

    ::SetDepthTest(true);

    ::SetCullFace(true);

    m_camera = new Camera(GetWindowWidth()*2,GetWindowHeight()*2,Vector3f(0,0.0f,0), Vector3f(1.0f,-0.5f,1.0f), true);

    m_fullscreenFbo = new ColorFBO();
    m_fullscreenFbo->Init(9, GetFramebufferWidth(), GetFramebufferHeight());

    m_quad = new Quad(Vector2f(-1.0f), Vector2f(1.0f));

    m_postShader = new ShaderProgram("shader/post");


    //                    128000
    m_skydome = new Skydome(1, 10, 10);
}

void ScatteringApplication::Render() {

    m_fullscreenFbo->Bind();
    {
	SetViewport();
	Clear(0.0f, 1.0f, 1.0f);
	m_skydome->Draw(*m_camera);
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

void ScatteringApplication::Update(const float delta) {
    m_camera->HandleInput(delta);

    m_skydome->Update(delta);
}

void ScatteringApplication::RenderText()  {
}
