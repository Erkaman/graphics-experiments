#include "cellular_application.hpp"

#include "ewa/camera.hpp"
#include "ewa/font.hpp"

#include "ewa/math/vector4f.hpp"


#include "plane.hpp"

using namespace std;

CellularApplication::CellularApplication(int argc, char *argv[]):Application(argc, argv), m_camera(NULL), m_plane(NULL){ }

CellularApplication::~CellularApplication() {
    MY_DELETE(m_camera);
    MY_DELETE(m_plane);
}

void CellularApplication::Init() {

    ::SetDepthTest(true);
    ::SetCullFace(true);

    m_camera = new Camera(GetWindowWidth(),GetWindowHeight(),Vector3f(0,7.0f,0), Vector3f(1.0f,-0.5f,1.0f), true);

    m_plane = new Plane(Vector3f(1,4,1), Vector3f(1,1,1));

}

void CellularApplication::Render() {

    SetViewport();
    Clear(0.0f, 1.0f, 1.0f);

    const Vector4f lightPosition(93,10.0f,93, 1.0f);
    m_plane->Draw(*m_camera, lightPosition);


}

void CellularApplication::Update(const float delta) {
    m_camera->HandleInput(delta);
}

void CellularApplication::RenderText()  {
    m_font->DrawString(*m_fontShader, 600,150, "hello world" );
}
