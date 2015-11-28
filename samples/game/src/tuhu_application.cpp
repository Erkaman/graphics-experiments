#include "tuhu_application.hpp"

#include "ewa/camera.hpp"
#include "ewa/common.hpp"
#include "ewa/font.hpp"
#include "ewa/keyboard_state.hpp"


#include "ewa/audio/sound.hpp"
#include "ewa/audio/wave_loader.hpp"

#include "skydome.hpp"
#include "height_map.hpp"
#include "grass.hpp"
#include "particle_system.hpp"
#include "smoke_effect.hpp"
#include "snow_effect.hpp"
#include "fire_effect.hpp"

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
    m_smoke = new SmokeEffect(Vector3f(11.5,-3,10));
    m_smoke->Init();

    ::SetDepthTest(true);
    ::SetCullFace(true);

    const Vector3f pos = Vector3f(5.997801, 5.711470, -3.929811);
    m_camera = new Camera(GetWindowWidth()*2,GetWindowHeight()*2,

		pos,Vector3f(-0.758915, -0.464640, 0.456243)

, true);



    m_snow = new SnowEffect(pos);
    m_snow->Init();



    m_fire = new FireEffect(Vector3f(12,-3,10));
    m_fire->Init();


    m_heightMap = new HeightMap("img/combined.png");



    //                    128000
    m_skydome = new Skydome(1, 10, 10);


    m_grass = new Grass(Vector2f(10,10), m_heightMap);


    m_stoneFloor = new GeometryObject();
    m_stoneFloor->Init("obj/rock_floor.eob");
    m_stoneFloor->SetModelMatrix(
	Matrix4f::CreateTranslation(Vector3f(0,0,0)));

    m_flatWoodFloor = new GeometryObject();
    m_flatWoodFloor->Init("obj/flat_wood_floor.eob");
    m_flatWoodFloor->SetModelMatrix(
	Matrix4f::CreateTranslation(Vector3f(10,0,0)));

    m_woodFloor = new GeometryObject();
    m_woodFloor->Init("obj/wood_floor.eob");
    m_woodFloor->SetModelMatrix(
	Matrix4f::CreateTranslation(Vector3f(-10,0,0)));

    m_sphere = new GeometryObject();
    m_sphere->Init("obj/sunball.eob");
    m_sphere->SetModelMatrix(
	Matrix4f::CreateTranslation(Vector3f(0,3,0)));



    /*
    OpenAL::Init();

    m_windSound = new Sound("audio/wind2.wav");
    m_windSound->SetGain(1.0f);
    m_windSound->SetLooping(true);
	*/
}

void TuhuApplication::Render() {

    SetViewport();

    Clear(0.0f, 1.0f, 1.0f);

    m_skydome->Draw(*m_camera);

    Vector4f lightPosition(93,10.0f,93, 1.0f);
   m_heightMap->Draw(*m_camera, lightPosition);

    m_grass->Draw(*m_camera, lightPosition);

    m_smoke->Render(m_camera->GetMvp(), m_camera->GetPosition());


	m_snow->Render(m_camera->GetMvp(), m_camera->GetPosition());

	m_fire->Render(m_camera->GetMvp(), m_camera->GetPosition());


    m_stoneFloor->Render(*m_camera, lightPosition);

    m_flatWoodFloor->Render(*m_camera, lightPosition);

    m_woodFloor->Render(*m_camera, lightPosition);

    m_sphere->SetModelMatrix(
	Matrix4f::CreateTranslation(Vector3f(0,3,0)));
    m_sphere->Render(*m_camera, lightPosition);



    m_sphere->SetModelMatrix(
	Matrix4f::CreateTranslation(Vector3f(lightPosition)));
    m_sphere->Render(*m_camera, lightPosition);

}

void TuhuApplication::Update(const float delta) {

    m_camera->HandleInput(delta);

    m_smoke->Update(delta);
    m_snow->Update(delta);
    m_fire->Update(delta);

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
