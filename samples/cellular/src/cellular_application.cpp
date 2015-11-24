#include "cellular_application.hpp"

#include "ewa/camera.hpp"
#include "ewa/font.hpp"

#include "ewa/math/vector4f.hpp"
#include "ewa/keyboard_state.hpp"

#include "plane.hpp"
#include "cube.hpp"

#include <math.h>

using namespace std;

float angle = 0;


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
	Matrix4f::CreateTranslation(Vector3f(0,0,0)));

    m_woodFloor = new GeometryObject();
    m_woodFloor->Init("obj/wood_floor.eob");
    m_woodFloor->SetModelMatrix(
	Matrix4f::CreateTranslation(Vector3f(10,0,0)));

    const float SCALE3 = 0.2f;

}

void CellularApplication::Render() {

    SetViewport();
    Clear(0.0f, 0.3f, 0.5f);

    angle += 0.2;

    m_stoneFloor->Render(*m_camera, m_lightPosition);

    m_woodFloor->Render(*m_camera, m_lightPosition);

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
    m_font->DrawString(*m_fontShader, 30,150, "The quick. brown, fox ijumps? over& the) lazy/ dog" );

    m_font->DrawString(*m_fontShader, 30,190, "THE QUICK BROWN FOX JUMPS OVER THE LAZY DOG" );

}
