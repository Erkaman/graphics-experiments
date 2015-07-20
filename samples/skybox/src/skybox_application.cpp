#include "skybox_application.hpp"

#include "ewa/camera.hpp"
#include "ewa/font.hpp"

#include "skybox.hpp"

using namespace std;

SkyboxApplication::SkyboxApplication(int argc, char *argv[]):Application(argc, argv), m_camera(NULL), m_skybox(NULL){ }

SkyboxApplication::~SkyboxApplication() {
    MY_DELETE(m_camera);
    MY_DELETE(m_skybox);
}

void SkyboxApplication::Init() {

    ::SetDepthTest(true);
    ::SetCullFace(true);

    m_camera = new Camera(GetWindowWidth()*2,GetWindowHeight()*2,Vector3f(0,0.0f,0), Vector3f(1.0f,-0.5f,1.0f), true);

    m_skybox = new Skybox(
	"img/bluecloud_ft.png",
	"img/bluecloud_bk.png",
	"img/bluecloud_lf.png",
	"img/bluecloud_rt.png",
	"img/bluecloud_up.png",
	"img/bluecloud_dn.png"
	);
}

void SkyboxApplication::Render() {

    SetViewport();
    Clear(0.0f, 1.0f, 1.0f);

    // render skybox

    m_skybox->Draw(*m_camera);
}

void SkyboxApplication::Update(const float delta) {
    m_camera->HandleInput(delta);
}

void SkyboxApplication::RenderText()  {
    m_font->DrawString(*m_fontShader, 600,150, "hello world" );
}
