#include "cellular_application.hpp"

#include "ewa/camera.hpp"
#include "ewa/font.hpp"

#include "ewa/math/vector4f.hpp"


#include "plane.hpp"
#include "cube.hpp"

using namespace std;

CellularApplication::CellularApplication(int argc, char *argv[]):Application(argc, argv), m_camera(NULL), m_cellularPlane(NULL), m_perlinPlane(NULL), m_normalCube(NULL) { }

CellularApplication::~CellularApplication() {
    MY_DELETE(m_camera);
    MY_DELETE(m_cellularPlane);
      MY_DELETE(m_perlinPlane);
      MY_DELETE(m_cube);
      MY_DELETE(m_normalCube);
      MY_DELETE(m_smileCube);
      MY_DELETE(m_stoneBall);

}

void CellularApplication::Init() {

    ::SetDepthTest(true);
    ::SetCullFace(true);

    m_camera = new Camera(GetWindowWidth(),GetWindowHeight(),Vector3f(0,0.0f,5), Vector3f(0.0f,0.0f,-1.0f), true);

/*    m_cellularPlane = new Plane(Vector3f(1,4,1), true);
    m_perlinPlane = new Plane(Vector3f(20,4,1), false);

    m_cube = new Cube(Vector3f(20,4,1));

    m_normalCube = new GeometryObject();
    m_normalCube->Init("obj/mybox2.eob");
    m_normalCube->SetModelMatrix(Matrix4f::CreateTranslation(Vector3f(10,6,1)));

    m_smileCube = new GeometryObject();
    m_smileCube->Init("obj/smile_cube.eob");
    m_smileCube->SetModelMatrix(Matrix4f::CreateTranslation(Vector3f(20,6,5)));

    m_stoneBall = new GeometryObject();
    m_stoneBall->Init("obj/stone_ball.eob");
    m_stoneBall->SetModelMatrix(Matrix4f::CreateTranslation(Vector3f(0,6,5)));

    m_floor = new GeometryObject();
    m_floor->Init("obj/floor.eob");
    m_floor->SetModelMatrix(Matrix4f::CreateTranslation(Vector3f(0,6,5)));
*/

    const float SCALE = 0.01f;

    m_door = new GeometryObject();
    m_door->Init("obj/jumbodoor.eob");
    m_door->SetModelMatrix(

	Matrix4f::CreateScale(Vector3f(SCALE,SCALE,1)) *
	Matrix4f::CreateTranslation(Vector3f(00,0,00)));


}

void CellularApplication::Render() {

    SetViewport();
    Clear(0.0f, 1.0f, 1.0f);

//    const Vector4f lightPosition(93,10.0f,93, 1.0f);
//    const Vector4f lightPosition(30,6, 7, 1.0f);

    const Vector4f lightPosition(0,0, 10, 1.0f);


/*
    m_cellularPlane->Render(*m_camera, lightPosition);

    m_perlinPlane->Render(*m_camera, lightPosition);


      m_cube->Render(*m_camera, lightPosition);


      m_normalCube->Render(*m_camera, lightPosition);

      m_smileCube->Render(*m_camera, lightPosition);

      m_stoneBall->Render(*m_camera, lightPosition);

      m_floor->Render(*m_camera, lightPosition);
*/
      m_door->Render(*m_camera, lightPosition);


}

void CellularApplication::Update(const float delta) {
    m_camera->HandleInput(delta);
}

void CellularApplication::RenderText()  {
    m_font->DrawString(*m_fontShader, 600,150, "hello world" );
}
