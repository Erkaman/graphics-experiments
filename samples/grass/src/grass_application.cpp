#include "grass_application.hpp"

#include "ewa/camera.hpp"
#include "ewa/font.hpp"

#include "ewa/math/vector4f.hpp"


#include "plane.hpp"
#include "grass.hpp"

using namespace std;

GrassApplication::GrassApplication(int argc, char *argv[]):Application(argc, argv), m_camera(NULL), m_plane(NULL) { }

GrassApplication::~GrassApplication() {
    MY_DELETE(m_camera);
    MY_DELETE(m_plane);
}

void GrassApplication::Init() {

    ::SetDepthTest(true);
    ::SetCullFace(true);

    m_camera = new Camera(GetWindowWidth(),GetWindowHeight(),Vector3f(0,7.0f,0), Vector3f(1.0f,-0.5f,1.0f), true);

    m_plane = new Plane(Vector3f(20,4,1), Vector3f(1,1,1));

    m_grass = new Grass();
}

void GrassApplication::Render() {

    SetViewport();
    Clear(0.0f, 1.0f, 1.0f);

    const Vector4f lightPosition(93,10.0f,93, 1.0f);

    m_plane->Draw(*m_camera, lightPosition);

    m_grass->Draw(*m_camera, lightPosition);


}

void GrassApplication::Update(const float delta) {
    m_camera->HandleInput(delta);
}

void GrassApplication::RenderText()  {
    m_font->DrawString(*m_fontShader, 600,150, "hello world" );
}
