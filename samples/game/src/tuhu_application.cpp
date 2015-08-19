#include "tuhu_application.hpp"

#include "ewa/camera.hpp"
#include "ewa/common.hpp"
#include "ewa/font.hpp"
#include "ewa/keyboard_state.hpp"

#include "ewa/audio/sound.hpp"

#include "skydome.hpp"
#include "height_map.hpp"
#include "grass.hpp"



using namespace std;

void ToClipboard(const std::string& str) {
    std::string command = "echo '" + str + "' | pbcopy";
    system(command.c_str());
}


TuhuApplication::TuhuApplication(int argc, char *argv[]):Application(argc, argv), m_camera(NULL), m_heightMap(NULL),m_skydome(NULL){ }

TuhuApplication::~TuhuApplication() {
    MY_DELETE(m_camera);
    MY_DELETE(m_heightMap);
    MY_DELETE(m_skydome);
    MY_DELETE(m_windSound);
}

void TuhuApplication::Init() {

    ::SetDepthTest(true);

    ::SetCullFace(true);

    m_camera = new Camera(GetWindowWidth()*2,GetWindowHeight()*2,

Vector3f(1.836625, -1.669451, -3.106014),Vector3f(-0.431896, -0.336571, 0.836771)


//Vector3f(9.474550, -3.490666, 9.546431),Vector3f(0.666667, -0.333333, 0.666667)


, true);

    m_heightMap = new HeightMap("img/combined.png");

    //                    128000
    m_skydome = new Skydome(1, 10, 10);

    m_grass = new Grass(Vector2f(10,10), m_heightMap);

    OpenAL::Init();

//    WaveLoader::Load("audio/click.wav");

    m_windSound = new Sound("audio/wind.wav");
    m_windSound->SetGain(1.0f);
    m_windSound->SetLooping(true);
}

void TuhuApplication::Render() {


    SetViewport();

    Clear(0.0f, 1.0f, 1.0f);

    m_skydome->Draw(*m_camera);

    Vector4f lightPosition(93,10.0f,93, 1.0f);
    m_heightMap->Draw(*m_camera, lightPosition);

    m_grass->Draw(*m_camera, lightPosition);
}

void TuhuApplication::Update(const float delta) {
    m_camera->HandleInput(delta);


    m_skydome->Update(delta);

    m_grass->Update(delta);

    const KeyboardState& kbs = KeyboardState::GetInstance();

    if( kbs.IsPressed(GLFW_KEY_P) ) {

	string out = "Vector3f" +tos(m_camera->GetPosition()) + ",";
	out += "Vector3f" + tos(m_camera->GetViewDir());
	ToClipboard(out);
    }

    static bool b= false;

    if( kbs.IsPressed(GLFW_KEY_B) && !b ) {
	LOG_I("play sound");
	m_windSound->Play();

	b = true;
    }
}

void TuhuApplication::RenderText()  {
    m_font->DrawString(*m_fontShader, 600,150, "hello world" );
}
