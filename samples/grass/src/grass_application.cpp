#include "grass_application.hpp"

#include "ewa/camera.hpp"
#include "ewa/font.hpp"
#include "ewa/keyboard_state.hpp"

#include "ewa/math/vector4f.hpp"


#include "plane.hpp"
#include "grass.hpp"

#include <stdlib.h>

using namespace std;

void ToClipboard(const std::string& str) {
    std::string command = "echo '" + str + "' | pbcopy";
    system(command.c_str());
}


GrassApplication::GrassApplication(int argc, char *argv[]):Application(argc, argv), m_camera(NULL), m_plane(NULL) { }

GrassApplication::~GrassApplication() {
    MY_DELETE(m_camera);
    MY_DELETE(m_plane);
}

void GrassApplication::Init() {

    ::SetDepthTest(true);
    ::SetCullFace(true);

    m_camera = new Camera(GetWindowWidth(),GetWindowHeight(),


			  Vector3f(6.672852, 5.344205, -1.718865), Vector3f(0.133544, 0.001992, 0.991041), true);

    m_plane = new Plane(Vector3f(5,4,1), Vector3f(1,1,1));

    m_grass = new Grass();
}

void GrassApplication::Render() {

    SetViewport();
    Clear(1.0f, 1.0f, 1.0f);

    const Vector4f lightPosition(7,4,5, 1.0f);

    m_plane->Draw(*m_camera, lightPosition);

    m_grass->Draw(*m_camera, lightPosition);
}

void GrassApplication::Update(const float delta) {
    m_camera->HandleInput(delta);

    const KeyboardState& kbs = KeyboardState::GetInstance();

    if( kbs.IsPressed(GLFW_KEY_P) ) {

	string out = "pos: " +tos(m_camera->GetPosition());
	out += "viewdir: " + tos(m_camera->GetViewDir());
	ToClipboard(out);

	LOG_I("hello");
    }
}

void GrassApplication::RenderText()  {
//    m_font->DrawString(*m_fontShader, 600,150, "hello world" );
/*
    m_font->DrawString(*m_fontShader, 100,150, tos(m_camera->GetPosition()).c_str() );
    m_font->DrawString(*m_fontShader, 100,250, tos(m_camera->GetViewDir()).c_str() );
*/
}
