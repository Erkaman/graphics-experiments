#include "tuhu_application.hpp"

#include "ewa/camera.hpp"
#include "ewa/height_map.hpp"
#include "ewa/common.hpp"
#include "ewa/font.hpp"

#include "skydome.hpp"



using namespace std;


TuhuApplication::TuhuApplication(int argc, char *argv[]):Application(argc, argv), m_camera(NULL), m_heightMap(NULL),m_skydome(NULL){ }

TuhuApplication::~TuhuApplication() {
    MY_DELETE(m_camera);
    MY_DELETE(m_heightMap);
    MY_DELETE(m_skydome);
}

void TuhuApplication::Init() {

    ::SetDepthTest(true);

    ::SetCullFace(true);

    m_camera = new Camera(GetWindowWidth()*2,GetWindowHeight()*2,Vector3f(0,0.0f,0), Vector3f(1.0f,-0.5f,1.0f), true);

    m_heightMap = new HeightMap("img/combined.png");



    //                    128000
    m_skydome = new Skydome(1, 10, 10);
}

void TuhuApplication::Render() {

    SetViewport();

    Clear(0.0f, 1.0f, 1.0f);

    m_skydome->Draw(*m_camera);

    Vector4f lightPosition(93,10.0f,93, 1.0f);
    m_heightMap->Draw(*m_camera, lightPosition);
}

void TuhuApplication::Update(const float delta) {
    m_camera->HandleInput(delta);

    //  m_skydome->Update(delta);
}

void TuhuApplication::RenderText()  {
    m_font->DrawString(*m_fontShader, 600,150, "hello world" );
}
