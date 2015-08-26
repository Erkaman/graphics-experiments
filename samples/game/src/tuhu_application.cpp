#include "tuhu_application.hpp"

#include "ewa/camera.hpp"
#include "ewa/common.hpp"
#include "ewa/font.hpp"
#include "ewa/keyboard_state.hpp"

#include "ewa/gl/texture2d.hpp"

#include "ewa/audio/sound.hpp"

#include "skydome.hpp"
#include "height_map.hpp"
#include "grass.hpp"
#include "particle_system.h"

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
    m_system = new ParticleSystem();

    constexpr float A = 0.02886f;
    m_system->SetMinVelocity(Vector3f(-A,A,-A));
    m_system->SetMaxVelocity(Vector3f(A,1.0f / 20.0f,A));
    m_system->SetMaxParticles(1000);
    m_system->SetEmitRate(0.1f);
    m_system->SetParticleLifetime(10.0f);
    m_system->SetBillboardSize(0.09f);

    Texture* texture = new Texture2D("img/smoke2.png");

    texture->Bind();
    texture->SetTextureRepeat();
    texture->GenerateMipmap();
    texture->SetMinFilter(GL_LINEAR_MIPMAP_LINEAR);
    texture->SetMagFilter(GL_LINEAR);
    texture->Unbind();

    m_system->SetTexture(texture);




    m_system->Init(Vector3f(10,-3,10));

    ::SetDepthTest(true);

    ::SetCullFace(true);

    m_camera = new Camera(GetWindowWidth()*2,GetWindowHeight()*2,
Vector3f(9.619988, -2.556720, 8.197560),Vector3f(0.239273, -0.065007, 0.968774)



, true);



    m_heightMap = new HeightMap("img/combined.png");



    //                    128000
    m_skydome = new Skydome(1, 10, 10);


    m_grass = new Grass(Vector2f(10,10), m_heightMap);



    OpenAL::Init();

//    WaveLoader::Load("audio/click.wav");

/*    m_windSound = new Sound("audio/wind2.wav");
    m_windSound->SetGain(1.0f);
    m_windSound->SetLooping(true);*/
}

void TuhuApplication::Render() {

    SetViewport();

    Clear(0.0f, 1.0f, 1.0f);

    m_skydome->Draw(*m_camera);

    Vector4f lightPosition(93,10.0f,93, 1.0f);
   m_heightMap->Draw(*m_camera, lightPosition);

    m_grass->Draw(*m_camera, lightPosition);

    m_system->Render(m_camera->GetMvp(), m_camera->GetPosition());
}

void TuhuApplication::Update(const float delta) {

    m_camera->HandleInput(delta);

    m_system->Update(delta);

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
