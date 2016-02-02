
#include "ao_application.hpp"

#include "ewa/common.hpp"
#include "ewa/camera.hpp"
#include "ewa/mouse_state.hpp"
#include "ewa/font.hpp"


/*
#include "ewa/keyboard_state.hpp"
#include "ewa/file.hpp"
#include "ewa/buffered_file_reader.hpp"
#include "ewa/string_util.hpp"
#include "ewa/resource_manager.hpp"
#include "ewa/timer.hpp"

#include "ewa/gl/depth_fbo.hpp"
#include "ewa/gl/texture.hpp"
#include "ewa/gl/cube_map_texture.hpp"

#include "ewa/audio/sound.hpp"
#include "ewa/audio/wave_loader.hpp"

#include "skydome.hpp"
#include "height_map.hpp"
#include "grass.hpp"
#include "particle_system.hpp"
#include "smoke_effect.hpp"
#include "snow_effect.hpp"
#include "fire_effect.hpp"
#include "ssao_pass.hpp"

#include "ewa/line.hpp"
#include "ewa/points.hpp"
#include "ewa/cube.hpp"
#include "ewa/view_frustum.hpp"

#include "ewa/config.hpp"

#include "ewa/physics_world.hpp"
#include "car_camera.hpp"

#include "picking_fbo.hpp"


#include "car.hpp"
#include "gui_enum.hpp"
#include "bt_util.hpp"

#include "gui.hpp"
#include "gui_mouse_state.hpp"

#include "gpu_profiler.hpp"
#include "physics_mask.hpp"
#include "gbuffer.hpp"

#include "skybox.hpp"
#include "env_camera.hpp"
#include "env_fbo.hpp"
*/


using namespace std;

int nonCulledObjects = 0;
int totalObjects = 0;

const string HEIGHT_MAP_FILENAME = "heightmap.bin";
const string SPLAT_MAP_FILENAME = "splatmap.bin";
const string AO_MAP_FILENAME = "aomap.bin";

const string OBJS_FILENAME = "objs";

void ToClipboard(const std::string& str) {
    std::string command = "echo '" + str + "' | pbcopy";
    system(command.c_str());
}

//(0.705072, 0.0758142, 0.705072)
AoApplication::AoApplication(int argc, char *argv[]):Application(argc, argv),
m_curCamera(NULL), m_lightDirection (

    Vector3f(-0.705072f, -0.958142f, -0.705072f).Normalize(),
//    -0.705072f, -0.0758142f, -0.705072f ,

    0.0f)   { }

AoApplication::~AoApplication() {
    MY_DELETE(m_freeCamera);
}

void AoApplication::Init() {


    /*
    m_gui = new Gui(m_window);
    m_gui->AddListener(this);
    GuiMouseState::Init(m_guiVerticalScale);
    */

    m_totalDelta = 0;

    // NOTE: we can fix the shadows by setting trans to (0,0,0).
    Vector3f trans = Vector3f(-2,9.0,-2);



    ::SetDepthTest(true);
    ::SetCullFace(true);


    const Vector3f pos =
	Vector3f(51, 41, 71);

    m_freeCamera = new Camera(
	GetFramebufferWidth(),
	GetFramebufferHeight(),
	pos,
	Vector3f(-0.50f, -0.50f, -0.50f)
	);

    m_curCamera = m_freeCamera;
}


void AoApplication::Render() {

    /*
    if(m_gui) {
	m_gui->NewFrame(m_guiVerticalScale);
    }
    */

    float SCALE = m_guiVerticalScale;

    // set the viewport to the size of the window.

    int windowWidth;
    int windowHeight;

    SetViewport();
    Clear(0.0f, 1.0f, 1.0f, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


//    GeometryObject::RenderAll(m_curCamera, m_lightDirection, lightVp, *m_depthFbo, m_envFbo->GetEnvMap());

    /*
    if(m_gui) {

	int fb_width, fb_height;
	fb_width = GetFramebufferWidth();
	fb_height = GetFramebufferHeight();

 	windowWidth = fb_width*SCALE * 0.5f;
	windowHeight = fb_height * 0.5f;


	m_gui->Render(windowWidth, windowHeight);
    }
    */
}

void AoApplication::Update(const float delta) {

    MouseState& ms = MouseState::GetInstance();

    /*
    if(m_gui)
	GuiMouseState::Update(GetFramebufferWidth(), GetFramebufferHeight());
    */

    m_totalDelta += delta;

    m_curCamera->Update(delta);

    /*
    if(m_gui)
	m_gui->Update();
    */

//    KeyboardState& kbs = KeyboardState::GetInstance();

}

string Format(const string& fmt, float val) {

    char buffer[30];

    sprintf(buffer,
	    fmt.c_str(),
	    val);

    return string(buffer);
}

void AoApplication::RenderText()  {

    string cull = std::to_string(nonCulledObjects) + "\\" + std::to_string(totalObjects);

    m_font->DrawString(*m_fontShader, 750,170, "lol");
}

/*
IGeometryObject* AoApplication::LoadObj(const std::string& path, const Vector3f& position,
					  const btQuaternion& rotation, float scale) {


    GeometryObject* obj = new GeometryObject();

//    LOG_I("add id: %d", currentObjId);
    bool result = obj->Init(path, position,rotation, scale, currentObjId++, COL_STATIC, staticCollidesWith);

    if(!result)
	PrintErrorExit();

    m_geoObjs[obj->GetId()] = obj;

    return obj;
}
*/

void AoApplication::Cleanup() {

//    string dir = Config::GetInstance().GetWorldFilename();


}
