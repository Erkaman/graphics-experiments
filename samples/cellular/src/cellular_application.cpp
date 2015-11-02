#include "cellular_application.hpp"

#include "ewa/camera.hpp"
#include "ewa/font.hpp"

#include "ewa/math/vector4f.hpp"
#include "ewa/keyboard_state.hpp"


#include "plane.hpp"
#include "cube.hpp"

#include <math.h>

using namespace std;

void ToClipboard(const std::string& str) {
    std::string command = "echo '" + str + "' | pbcopy";
    system(command.c_str());
}

CellularApplication::CellularApplication(int argc, char *argv[]):Application(argc, argv), m_camera(NULL), m_cellularPlane(NULL), m_perlinPlane(NULL), m_lightPosition(Vector4f(9,5, -15, 1.0f)), m_time(0.0){ }

CellularApplication::~CellularApplication() {
    MY_DELETE(m_camera);
/*    MY_DELETE(m_cellularPlane);
      MY_DELETE(m_perlinPlane);*/

      MY_DELETE(m_sun);
      MY_DELETE(m_ball);

}

void CellularApplication::Init() {

    ::SetDepthTest(true);
    ::SetCullFace(true);

    m_camera = new Camera(GetWindowWidth(),GetWindowHeight(),
Vector3f(-0.50, 1.0, 1.50),Vector3f(0.0, -0.50, -1.0)

, true);

    const float SCALE2 = 0.2f;

    m_stoneFloor = new GeometryObject();
    m_stoneFloor->Init("obj/rock_floor.eob");
    m_stoneFloor->SetModelMatrix(
	Matrix4f::CreateScale(Vector3f(SCALE2,1,SCALE2)) *
	Matrix4f::CreateTranslation(Vector3f(00,0,00)));


    const float SCALE3 = 0.2f;

    m_sun = new GeometryObject();
    m_sun->Init("obj/sunball.eob");
    m_sun->SetModelMatrix(
	Matrix4f::CreateScale(Vector3f(SCALE3,SCALE3,SCALE3)) *
	Matrix4f::CreateTranslation(Vector3f(m_lightPosition)  ));
/*
    m_ball = new GeometryObject();
    m_ball->Init("obj/sunball.eob");
    m_ball->SetModelMatrix(
	Matrix4f::CreateScale(Vector3f(SCALE3,SCALE3,SCALE3)) *
	Matrix4f::CreateTranslation(Vector3f(0.0, 2.0, 0.0)  ));
*/
}

void CellularApplication::Render() {

    SetViewport();
    Clear(0.0f, 1.0f, 1.0f);

//    const Vector4f lightPosition(93,10.0f,93, 1.0f);
//    const Vector4f lightPosition(30,6, 7, 1.0f);

//    const Vector4f lightPosition(0,0, 3, 8.0f);


    m_stoneFloor->Render(*m_camera, m_lightPosition);

    m_sun->Render(*m_camera, m_lightPosition);
//    m_ball->Render(*m_camera, m_lightPosition);
}

void CellularApplication::Update(const float delta) {
    m_camera->HandleInput(delta);

    const KeyboardState& kbs = KeyboardState::GetInstance();

    if( kbs.IsPressed(GLFW_KEY_P) ) {

	string out = "Vector3f" +tos(m_camera->GetPosition()) + ",";
	out += "Vector3f" + tos(m_camera->GetViewDir());
	ToClipboard(out);
    }
}

void CellularApplication::RenderText()  {
    m_font->DrawString(*m_fontShader, 600,150, "hello world" );
}

/*
the first tangent is (-1,0,0)

 */
