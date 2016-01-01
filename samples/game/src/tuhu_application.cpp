#include "tuhu_application.hpp"

#include "ewa/camera.hpp"
#include "ewa/common.hpp"
#include "ewa/font.hpp"
#include "ewa/keyboard_state.hpp"
#include "ewa/mouse_state.hpp"
#include "ewa/file.hpp"
#include "ewa/buffered_file_reader.hpp"
#include "ewa/string_util.hpp"
#include "ewa/resource_manager.hpp"
#include "ewa/timer.hpp"


#include "ewa/gl/depth_fbo.hpp"
#include "ewa/gl/texture.hpp"

#include "ewa/audio/sound.hpp"
#include "ewa/audio/wave_loader.hpp"

#include "skydome.hpp"
#include "height_map.hpp"
#include "grass.hpp"
#include "particle_system.hpp"
#include "smoke_effect.hpp"
#include "snow_effect.hpp"
#include "fire_effect.hpp"

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


using namespace std;

int nonCulledObjects = 0;
int totalObjects = 0;

constexpr int SHADOW_MAP_SIZE = 1024;

const string HEIGHT_MAP_FILENAME = "heightmap.bin";
const string SPLAT_MAP_FILENAME = "splatmap.bin";
const string OBJS_FILENAME = "objs";

void ToClipboard(const std::string& str) {
    std::string command = "echo '" + str + "' | pbcopy";
    system(command.c_str());
}

//(0.705072, 0.0758142, 0.705072)
TuhuApplication::TuhuApplication(int argc, char *argv[]):Application(argc, argv),
m_curCamera(NULL), m_heightMap(NULL), m_skydome(NULL), m_gui(NULL), m_lightDirection (

    -0.705072f, -0.458142f, -0.705072f,
//    -0.705072f, -0.0758142f, -0.705072f ,

    0.0f), m_pickingFbo(NULL)   { }

TuhuApplication::~TuhuApplication() {
    MY_DELETE(m_carCamera);
    MY_DELETE(m_freeCamera);

    MY_DELETE(m_heightMap);
    MY_DELETE(m_skydome);
    MY_DELETE(m_windSound);
    MY_DELETE(m_gui);

    MY_DELETE(m_depthFbo);
    MY_DELETE(m_pickingFbo);

}

void TuhuApplication::Init() {

    currentObjId = 0;

    Config& m_config = Config::GetInstance();
    if(m_config.IsGui()) {

	m_gui = new Gui(m_window);
	m_gui->AddListener(this);

	GuiMouseState::Init(m_guiVerticalScale);

    } else {

	LOG_I("No GUI created");
    }


    m_physicsWorld = new PhysicsWorld();

    m_viewFrustum = new ViewFrustum();

    m_totalDelta = 0;

    // NOTE: we can fix the shadows by setting trans to (0,0,0).
    Vector3f trans = Vector3f(-130,5,-130);

    m_smoke = new SmokeEffect(Vector3f(10,3,10) + trans);
    m_smoke->Init();

    ::SetDepthTest(true);
    ::SetCullFace(true);


    const Vector3f pos =
	Vector3f(51.479908f, 40.918278f, 70.826126f);

    m_freeCamera = new Camera(
	GetFramebufferWidth(),
	GetFramebufferHeight(),
	pos,
	Vector3f(-0.613098f, -0.523130f, -0.591984f)
	);

    m_snow = new SnowEffect(pos);
    m_snow->Init();



    m_fire = new FireEffect(Vector3f(12,-3,10));
    m_fire->Init();


    m_skydome = new Skydome(1, 10, 10);


    string dir = Config::GetInstance().GetWorldFilename();


    m_car = new Car();
    bool result = m_car->Init(Vector3f(0,-1.5,0)+trans);

    if(!result) {
	LOG_I("could not load car");
	PrintErrorExit();
    }
    m_geoObjs.push_back(m_car);

    bool guiMode = (m_gui != 0);

    if(ResourceManager::GetInstance().PathExists(dir) && false) {

	m_heightMap = new HeightMap(
	    File::AppendPaths(dir, HEIGHT_MAP_FILENAME ) ,
	    File::AppendPaths(dir, SPLAT_MAP_FILENAME ), guiMode);

	ParseObjs(File::AppendPaths(dir, OBJS_FILENAME ));

	LOG_I("loaded heightmap");


    } else {

	m_heightMap = new HeightMap(guiMode);

	LoadObj("obj/rock_floor.eob", Vector3f(0,0,40) + trans);

	LoadObj("obj/flat_wood_floor.eob", Vector3f(10,0,40)+ trans );

	LoadObj("obj/wood_floor.eob", Vector3f(-10,0,40)+ trans );

	LoadObj("obj/sunball.eob",

			   Vector3f(21.152159f, 13.744261f, 21.152159f)+ trans  + Vector3f(60,0,60) );

	LoadObj("obj/plane.eob", Vector3f(0,-2.5,0)+ trans);

	IGeometryObject* tree = LoadObj("obj/tree.eob", Vector3f(10,-2.5,10) + trans);

	LoadObj("obj/wall.eob", Vector3f(-5,-2.5,-5)  + trans);

	LoadObj("obj/wall.eob", Vector3f(20,-6.5,-5) + trans);

	m_selected = tree;
	m_selected->SetSelected(true);

	/*m_selected = */LoadObj("obj/wall.eob",
				 Vector3f(29.152159f, 13.744261f, 21.152159f)+ trans  + Vector3f(60,0,60)
	    );

    }

    if(m_gui) {
	m_heightMap->SetCursorSize(m_gui->GetCursorSize());

	// do a first update, in order to setup the PBOs
	m_heightMap->Update(0, NULL, 0,0);
	m_heightMap->Update(0, NULL, 0,0);

    }


    m_depthFbo = new DepthFBO();

    // TODO: should not this be the size of the framebuffer?
    m_depthFbo->Init(DEPTH_FBO_TEXTURE_UNIT, SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);

    m_carCamera = new CarCamera(GetFramebufferWidth(),GetFramebufferHeight(),
				m_car
	);

    if(m_gui) {
	m_pickingFbo = new PickingFBO();

	// TODO: should not this be the size of the framebuffer?
	m_pickingFbo->Init(PICKING_FBO_TEXTURE_UNIT, GetFramebufferWidth(),GetFramebufferHeight() );
    }
    /*
      OpenAL::Initp();

      m_windSound = new Sound("audio/wind2.wav");
      m_windSound->SetGain(1.0f);
      m_windSound->SetLooping(true);
    */

    m_curCamera = m_freeCamera;


//    StartPhysics();

}


Matrix4f TuhuApplication::MakeLightProj()const {

    // TODO: does not yet fully work.
    //http://www.gamedev.net/topic/505893-orthographic-projection-for-shadow-mapping/


    /*
      First we compute the world space location of all 8 corners of the view frustum.
    */

    Matrix4f invProj = m_curCamera->GetVp().Inverse();

    // left bottom near
    const Vector3f lbf = Vector3f((invProj * Vector4f(-1,-1,-1,1.0f)));



/*
  LOG_I("sanity: %s",
  string(m_camera->GetMvpFromM() * Vector4f(17.328205, 15.360136, 14.091190, 1.0)).c_str()

  );

  exit(1);*/

//    	Vector3f(17.328205, 15.360136, 14.091190);


    // left top near
    const Vector3f ltf = Vector3f((invProj * Vector4f(-1,+1,-1,1.0f)));

    // right bottom near
    const Vector3f rbf = Vector3f((invProj * Vector4f(+1,-1,-1,1.0f)));

    // right top near
    const Vector3f rtf = Vector3f((invProj * Vector4f(+1,+1,-1,1.0f)));

    // left bottom far
    const Vector3f lbn = Vector3f((invProj * Vector4f(-1,-1,+1,1.0f)));




    // left top far
    const Vector3f ltn = Vector3f((invProj * Vector4f(-1,+1,+1,1.0f)));


    // right bottom far
    const Vector3f rbn = Vector3f((invProj * Vector4f(+1,-1,+1,1.0f)));

    // right top far
    const Vector3f rtn = Vector3f((invProj * Vector4f(+1,+1,+1,1.0f)));


    // LOG_I("lbf: %s",  string(lbf).c_str() );
    //  LOG_I("lbn: %s",  string(lbn).c_str() );

    Vector3f corners[8] =  {lbn , ltn , rbn , rtn ,
			    lbf , ltf , rbf , rtf };

    Vector3f centroid = Vector3f(0,0,0);
    for(int i = 0; i < 8; ++i) {
	centroid += corners[i];
    }
    centroid = centroid * (1.0f/8.0f);

    //  LOG_I("centroid %s", string(centroid).c_str() );



    Config& config = Config::GetInstance();


    const float nearClipOffset = 50.0f;


    Matrix4f viewMatrix =
	Matrix4f::CreateLookAt(
	    centroid + ( Vector3f(m_lightDirection) * (config.GetZFar() + nearClipOffset )   ),
	    centroid,
	    Vector3f(0,1,0));


    // the frustum corners in lightspace.
    Vector3f cornersLS[8];

    for(int i = 0; i < 8; ++i) {
	cornersLS[i] = Vector3f(viewMatrix * Vector4f(corners[i], 1.0));
    }


    Vector3f mins = cornersLS[0];

    Vector3f maxes = cornersLS[0];

    for (int i = 0; i < 8; i++){

	if (cornersLS[i].x > maxes.x)
	    maxes.x = cornersLS[i].x;
	else if (cornersLS[i].x < mins.x)
	    mins.x = cornersLS[i].x;

	if (cornersLS[i].y > maxes.y)
	    maxes.y = cornersLS[i].y;
	else if (cornersLS[i].y < mins.y)
	    mins.y = cornersLS[i].y;

	if (cornersLS[i].z > maxes.z)
	    maxes.z = cornersLS[i].z;
	else if (cornersLS[i].z < mins.z)
	    mins.z = cornersLS[i].z;
    }

/*
  LOG_I("min x %f",  mins.x ); // -12
  LOG_I("max x %f",  maxes.x  ); // 12

  LOG_I("min y %f",  mins.y ); // -23
  LOG_I("max y %f",  maxes.y  ); // 23

  LOG_I("-mimin z %f", -mins.z   ); // 2
  LOG_I("mymaxz %f",  maxes.z ); // -23

//    exit(1);*/

    // TODO: the problem probably lies in that we should flip maxz and minz or something.
    return Matrix4f::CreateOrthographic(mins.x, maxes.x, mins.y, maxes.y, maxes.z, -mins.z );
}


void TuhuApplication::RenderShadowMap() {

/*
  m_depthFbo->Bind();
  {
  ::SetViewport(0,0,SHADOW_MAP_SIZE,SHADOW_MAP_SIZE);

  Clear(0.0f, 1.0f, 1.0f, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  m_lightViewMatrix = Matrix4f::CreateLookAt(
  -Vector3f(m_lightDirection),
  Vector3f(0.0f, 0.0f, 0.0f),
  Vector3f(0.0, 1.0, 0.0)
  );


  m_lightProjectionMatrix =  //MakeLightProj();
  Matrix4f::CreateOrthographic(-30, 30, -12, 12, -20, 30);

  Matrix4f vp = m_lightProjectionMatrix * m_lightViewMatrix;

  m_sphere->RenderShadowMap(vp);

  m_tree->RenderShadowMap(vp);

  m_wall->RenderShadowMap(vp);
  m_wall2->RenderShadowMap(vp);


  }
  m_depthFbo->Unbind();
*/
}

void TuhuApplication::RenderId() {

    m_pickingFbo->Bind();

    Clear(0.0f, 0.0f, 0.0f, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // set viewport
    m_heightMap->RenderId(m_curCamera);

    GeometryObject::RenderIdAll(m_curCamera);

/*
    for(IGeometryObject* geoObj: m_geoObjs) {

	if(m_viewFrustum->IsAABBInFrustum(geoObj->GetModelSpaceAABB())) {

	    if(geoObj != m_car) {
		geoObj->RenderId(m_curCamera);
	    }

	}
    }
*/

    m_pickingFbo->Unbind();


/*
  GL_C(glBindFramebuffer(GL_FRAMEBUFFER, 0));

  m_pickingFbo->UnbindForWriting();
  m_pickingFbo->BindForReading();
  GL_C(glReadBuffer(GL_COLOR_ATTACHMENT0));


  // bl it

  int w = GetFramebufferWidth();
  int h = GetFramebufferHeight();


  GL_C(glBlitFramebuffer(
  0, 0, w, h, 0, 0, w, h, GL_COLOR_BUFFER_BIT, GL_NEAREST
  ));

  m_pickingFbo->UnbindForReading();


  LOG_I("actual framebuffeR: %d, %d", GetFramebufferWidth(), GetFramebufferHeight() );
*/

//    m_pickingFbo->GetRenderTargetTexture().WriteIdToFile("id.png");
//    m_pickingFbo->GetRenderTargetTexture().WriteToFile("id.png");

//    exit(1);
}

void TuhuApplication::RenderScene() {

    m_skydome->Draw(m_curCamera);

    m_heightMap->Render(m_curCamera, m_lightDirection);

//    m_grass->Draw(m_curCamera, m_lightDirection);



    //  m_snow->Render(m_curCamera->GetMvpFromM(), m_curCamera->GetPosition());

    //   m_fire->Render(m_curCamera->GetVp(), m_curCamera->GetPosition());


    Matrix4f biasMatrix(
	0.5f, 0.0f, 0.0f, 0.5f,
	0.0f, 0.5f, 0.0f, 0.5f,
	0.0f, 0.0f, 0.5f, 0.5f,
	0.0f, 0.0f, 0.0f, 1.0f
	);

    Matrix4f lightVp =  biasMatrix *  m_lightProjectionMatrix * m_lightViewMatrix;

    GeometryObject::RenderAll(m_curCamera, m_lightDirection, lightVp, *m_depthFbo);

    /*
    nonCulledObjects = 0;
    for(IGeometryObject* geoObj: m_geoObjs) {

	if(m_viewFrustum->IsAABBInFrustum(geoObj->GetModelSpaceAABB())) {
	    ++nonCulledObjects;

	    if(m_selected && m_selected == geoObj) {
		geoObj->RenderWithOutlines( m_curCamera, m_lightDirection, lightVp, *m_depthFbo);
	    } else {
		geoObj->Render(m_curCamera, m_lightDirection, lightVp, *m_depthFbo);
	    }

	}
    }

    totalObjects = m_geoObjs.size();
    */


    m_smoke->Render(m_curCamera->GetVp(), m_curCamera->GetPosition());

}

void TuhuApplication::Render() {

    if(m_gui) {
	m_gui->NewFrame(m_guiVerticalScale);
    }

    RenderShadowMap();

    float SCALE = m_guiVerticalScale;

    // set the viewport to the size of the window.

    int windowWidth;
    int windowHeight;

    SetViewport();

    Clear(0.0f, 1.0f, 1.0f, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if(m_pickingFbo)
	RenderId();

    RenderScene();


    if(m_gui) {

	int fb_width, fb_height;
	fb_width = GetFramebufferWidth();
	fb_height = GetFramebufferHeight();

 	windowWidth = fb_width*SCALE * 0.5f;
	windowHeight = fb_height * 0.5f;


	m_gui->Render(windowWidth, windowHeight);
    }

}

void TuhuApplication::Update(const float delta) {

    MouseState& ms = MouseState::GetInstance();

    if(m_gui)
	GuiMouseState::Update(GetFramebufferWidth(), GetFramebufferHeight());



    m_viewFrustum->Update( m_curCamera->GetVp() );

    m_physicsWorld->Update(delta);

    for(IGeometryObject* geoObj: m_geoObjs) {

	geoObj->Update(*m_viewFrustum);

    }

    m_totalDelta += delta;

    m_curCamera->Update(delta);

    m_smoke->Update(delta);
/*    //   m_snow->Update(delta);
      m_fire->Update(delta);
*/
    m_skydome->Update(delta);

//      m_grass->Update(delta);

    if(m_gui)
	m_gui->Update();

    KeyboardState& kbs = KeyboardState::GetInstance();

/*
  if(input_accepted) {
  add to object.
  }
*/

    if( kbs.IsPressed(GLFW_KEY_P) ) {

	string out = "Vector3f" +tos(m_curCamera->GetPosition()) + ",";
	out += "Vector3f" + tos(m_curCamera->GetViewDir());
	ToClipboard(out);
    }

    /*
      if( kbs.IsPressed(GLFW_KEY_7) ) {
      m_heightMap->SaveHeightMap();
      }

      if( kbs.IsPressed(GLFW_KEY_8) ) {
      m_heightMap->SaveSplatMap();
      }
    */

    if( kbs.WasPressed(GLFW_KEY_6)/* && !m_gui*/ ) {
	StartPhysics();
    }

    if(GuiMouseState::isWithinWindow()) {

	if(m_gui->GetGuiMode() == ModifyTerrainMode) {

	    if(ms.IsPressed(GLFW_MOUSE_BUTTON_1 )) {

		if(m_gui->GetTerrainMode() == ModifyElevationMode)
		    m_heightMap->ModifyTerrain(delta, +m_gui->GetStrength()  );
		else if(m_gui->GetTerrainMode() == DistortMode){
		    m_heightMap->DistortTerrain(delta, +m_gui->GetStrength(), m_gui->GetNoiseScale()  );
		} else if(m_gui->GetTerrainMode() == SmoothMode) {

		    m_heightMap->SmoothTerrain(delta, m_gui->GetSmoothRadius() );
		} else {
		    // flat.
		    m_heightMap->LevelTerrain(delta , m_gui->GetStrength());
		}
	    }

	    if(ms.IsPressed(GLFW_MOUSE_BUTTON_2 )) {

		if(m_gui->GetTerrainMode() == ModifyElevationMode)
		    m_heightMap->ModifyTerrain(delta, -m_gui->GetStrength() );
	    }

	} else if(m_gui->GetGuiMode() == DrawTextureMode) {


	    if(ms.IsPressed(GLFW_MOUSE_BUTTON_1 )) {
		m_heightMap->DrawTexture(delta, m_gui->GetDrawTextureType() );
	    }
	}
    }


    if(ms.WasPressed(GLFW_MOUSE_BUTTON_1 )) {


	if(m_gui->GetGuiMode() == ModelMode) {

	    float y = GetFramebufferHeight() - GuiMouseState::GetY() - 1;
	    float x = GuiMouseState::GetX();

	    PixelInfo pi = m_pickingFbo->ReadPixel((unsigned int)x,(unsigned int)y);

	    unsigned int id = (unsigned int)pi.id;

	    if(id != 0) {
//		LOG_I("triangle: %f, %f", pi.unused1, pi.id);

		if(m_selected){
		    // deselected formerly selected object.
		    m_selected->SetSelected(false);
		}

		m_selected = m_geoObjs[id];
		m_selected->SetSelected(true);

	    }


	}


    }


    if(m_gui && m_gui->GetGuiMode() == ModelMode && m_selected ) {

	m_selected->SetEditPosition( m_gui->GetTranslation() );
	m_selected->SetEditRotation( toBtQuat(m_gui->GetRotation()) );

    }


    static bool b= false;

    if( kbs.IsPressed(GLFW_KEY_B) && !b ) {
	LOG_I("play sound");
	m_windSound->Play();

	b = true;
    }

    if( kbs.WasPressed(GLFW_KEY_N) ) {

	if(m_curCamera == m_freeCamera) {
	    m_curCamera = m_carCamera;

	} else {
	    m_curCamera = m_freeCamera;
	}
	m_curCamera->Update(0);
    }

    m_heightMap->Update(delta, m_curCamera, (float)GetFramebufferWidth(),(float)GetFramebufferHeight());

}

void TuhuApplication::RenderText()  {

    string cull = std::to_string(nonCulledObjects) + "\\" + std::to_string(totalObjects);

    m_font->DrawString(*m_fontShader, 750,170, cull);

    m_font->DrawString(*m_fontShader, 750,120, tos(m_curCamera->GetPosition())  );

//    m_font->DrawString(*m_fontShader, 600,120, cull );
}

IGeometryObject* TuhuApplication::LoadObj(const std::string& path, const Vector3f& position,
					  const btQuaternion& rotation) {


    GeometryObject* obj = new GeometryObject();

    bool result = obj->Init(path, position,rotation, currentObjId++);

    if(!result)
	PrintErrorExit();

    m_geoObjs.push_back(obj);

    return obj;
}

void TuhuApplication::StartPhysics()  {

    for(IGeometryObject* geoObj: m_geoObjs) {
	geoObj->AddToPhysicsWorld(m_physicsWorld);
    }

    m_heightMap->AddToPhysicsWorld(m_physicsWorld);

}

void TuhuApplication::Cleanup() {

    if(m_gui) {
	// if we were in the world editor, we need to serialize the world.

	string dir = Config::GetInstance().GetWorldFilename();

	// we save the entire world in a directory. Make sure the directory exists:
	File::CreatePath(dir);

	// save heightmap:
	m_heightMap->SaveHeightMap(File::AppendPaths(dir, HEIGHT_MAP_FILENAME ) );

	// save splatmap:
	m_heightMap->SaveSplatMap(File::AppendPaths(dir, SPLAT_MAP_FILENAME ) );

	File* outFile = File::Load(
	    File::AppendPaths(dir, OBJS_FILENAME ),
	    FileModeWriting);

	// -1, since we do not count the car. m_car
	outFile->WriteLine("numObjs " + to_string(m_geoObjs.size()-1) );


	for(IGeometryObject* geoObj: m_geoObjs) {

	    if(geoObj != m_car ) {
//		LOG_I("geoOjb: %s",  geoObj->GetFilename().c_str() );

		outFile->WriteLine("beginObj");

		outFile->WriteLine("filename " + geoObj->GetFilename() );

		Vector3f p = geoObj->GetPosition();
		outFile->WriteLine("translation " +
				   to_string(p.x) + " " +
				   to_string(p.y) + " " +
				   to_string(p.z)
		    );

		btQuaternion  q = geoObj->GetRotation();
		outFile->WriteLine("rotation " +
				   to_string(q.x() ) + " " +
				   to_string(q.y() ) + " " +
				   to_string(q.z() ) + " " +
				   to_string(q.w() ) + " "

		    );


		outFile->WriteLine("endObj");



	    }

	}

	delete outFile;

    }

    LOG_I("cleanup");
}

void TuhuApplication::ParseObjs(const std::string& filename) {

    BufferedFileReader* reader = BufferedFileReader::Load(  filename);
    if(!reader) {
	PrintErrorExit();
    }

    string firstLine = reader->ReadLine();

    size_t numObjs = stoi(StringUtil::SplitString(firstLine, " ")[1]);

    for(size_t iObj = 0; iObj < numObjs; ++iObj) {

	reader->ReadLine(); // beginObj

	string filenameLine = reader->ReadLine();
	string filename = StringUtil::SplitString(filenameLine, " ")[1];

	vector<string> tokens = StringUtil::SplitString(reader->ReadLine(), " ");
	Vector3f translation = Vector3f(stof(tokens[1]),stof(tokens[2]), stof(tokens[3]) );

	tokens = StringUtil::SplitString(reader->ReadLine(), " ");
	btQuaternion rotation = btQuaternion(
	    stof(tokens[1]),
	    stof(tokens[2]),
	    stof(tokens[3]),
	    stof(tokens[4]));

	reader->ReadLine(); // endObj

	// parsed the object. now add it to the world.
	LoadObj(filename, translation, rotation);

    }
}


void TuhuApplication::TranslationAccepted() {
    m_selected->SetPosition( m_gui->GetTranslation() + m_selected->GetPosition() );
    m_selected->SetEditPosition( Vector3f(0.0f) );
}

void TuhuApplication::RotationAccepted() {
    m_selected->SetRotation(

	m_selected->GetRotation() * toBtQuat(m_gui->GetRotation()));
    m_selected->SetEditRotation( btQuaternion::getIdentity()  );

}

void TuhuApplication::ModelAdded(const std::string& filename) {

    m_selected->SetSelected(false);

    m_selected = LoadObj(filename, Vector3f(0,0,0) );
    m_selected->SetSelected(true);
}

void TuhuApplication::CursorSizeChanged() {
    m_heightMap->SetCursorSize(m_gui->GetCursorSize());
}

void TuhuApplication::Duplicate() {
    if(m_selected) {


	IGeometryObject* dupObj = m_selected->Duplicate(currentObjId++);

	// select duplicated object:
	m_selected->SetSelected(false);
	dupObj->SetSelected(true);
	m_selected = dupObj;



	m_geoObjs.push_back(dupObj);


    }
}
