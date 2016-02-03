
#include "ao_application.hpp"
#include "ao_gui.hpp"
#include "simple_render.hpp"

#include "ewa/common.hpp"
#include "ewa/camera.hpp"
#include "ewa/mouse_state.hpp"
#include "ewa/font.hpp"
#include "ewa/geometry_object.hpp"
#include "ewa/gui_mouse_state.hpp"
#include "ewa/eob_file.hpp"


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

    m_gui = new AoGui(m_window, this);

    m_render = new SimpleRender();


    GuiMouseState::Init(m_guiVerticalScale);

    m_totalDelta = 0;

    // NOTE: we can fix the shadows by setting trans to (0,0,0).
    Vector3f trans = Vector3f(-2,9.0,-2);



    m_render->SetEob(EobFile::Read("obj/tree3_done.eob"), "obj" );
//    LoadObj("obj/tree3_done.eob");

    ::SetDepthTest(true);
    ::SetCullFace(true);

    const Vector3f pos =
	Vector3f(10, 10, 10);

    m_freeCamera = new Camera(
	GetFramebufferWidth(),
	GetFramebufferHeight(),
	pos,
	Vector3f(-0.50f, -0.50f, -0.50f)
	);

    m_curCamera = m_freeCamera;
}


void AoApplication::Render() {


    if(m_gui) {
	m_gui->NewFrame(m_guiVerticalScale);
    }


    float SCALE = m_guiVerticalScale;

    // set the viewport to the size of the window.

    int windowWidth;
    int windowHeight;

    SetViewport();
    Clear(0.0f, 1.0f, 1.0f, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//    GeometryObject::RenderSimple(m_curCamera, m_lightDirection);

    m_render->Render(m_curCamera, m_lightDirection);



	int fb_width, fb_height;
	fb_width = GetFramebufferWidth();
	fb_height = GetFramebufferHeight();

 	windowWidth = fb_width*SCALE * 0.5f;
	windowHeight = fb_height * 0.5f;


	m_gui->Render(windowWidth, windowHeight);

}

void AoApplication::Update(const float delta) {

    MouseState& ms = MouseState::GetInstance();


	GuiMouseState::Update(GetFramebufferWidth(), GetFramebufferHeight());


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


void AoApplication::LoadObj(const std::string& path) {

    m_geoObj = new GeometryObject();

    const Vector3f position(0,0,0);
    const btQuaternion rotation = btQuaternion::getIdentity();
    float scale = 1.0;

//    LOG_I("add id: %d", currentObjId);

    bool result = m_geoObj->Init(path, position,rotation, scale, 0,
				 0,0);

    if(!result)
	PrintErrorExit();
}


void AoApplication::Cleanup() {

//    string dir = Config::GetInstance().GetWorldFilename();


}


void AoApplication::BakeAo() {
    LOG_I("bake AO");
}
