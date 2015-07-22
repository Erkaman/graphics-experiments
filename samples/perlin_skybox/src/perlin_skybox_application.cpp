#include "perlin_skybox_application.hpp"

#include "ewa/camera.hpp"
#include "ewa/common.hpp"
#include "ewa/font.hpp"

#include "skydome.hpp"


using namespace std;


PerlinSkyboxApplication::PerlinSkyboxApplication(int argc, char *argv[]):Application(argc, argv), m_camera(NULL),m_skydome(NULL){ }

PerlinSkyboxApplication::~PerlinSkyboxApplication() {
    MY_DELETE(m_camera);
    MY_DELETE(m_skydome);
}

void PerlinSkyboxApplication::Init() {

    ::SetDepthTest(true);

    ::SetCullFace(true);

    m_camera = new Camera(GetWindowWidth()*2,GetWindowHeight()*2,Vector3f(0,0.0f,0), Vector3f(1.0f,-0.5f,1.0f), true);

    m_skydome = new Skydome(1, 10, 10);
}

void PerlinSkyboxApplication::Render() {
    SetViewport();

    Clear(0.0f, 1.0f, 1.0f);

    m_skydome->Draw(*m_camera);
}

void PerlinSkyboxApplication::Update(const float delta) {
    m_camera->HandleInput(delta);

    m_skydome->Update(delta);
}

void PerlinSkyboxApplication::RenderText()  {
    m_font->DrawString(*m_fontShader, 600,150, "hello world" );
}
