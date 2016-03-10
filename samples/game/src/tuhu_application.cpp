
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
#include "lighting_pass.hpp"

#include "ewa/line.hpp"
#include "ewa/points.hpp"
#include "ewa/cube.hpp"
#include "ewa/view_frustum.hpp"

#include "ewa/config.hpp"

#include "ewa/physics_world.hpp"
#include "car_camera.hpp"

#include "picking_fbo.hpp"
#include "ewa/gl/color_depth_fbo.hpp"

#include "ewa/gl/color_fbo.hpp"


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
#include "grid.hpp"



using namespace std;

int nonCulledObjects = 0;
int totalObjects = 0;

constexpr int SHADOW_MAP_SIZE =
    HighQuality ?
    1024*2*2 :
    1024*2;



constexpr int WINDOW_WIDTH = HighQuality ? 1500 : 1200;
constexpr int WINDOW_HEIGHT = HighQuality ?  960 : 720;

//constexpr int WINDOW_WIDTH = HighQuality ? 1500 : 720.0;
//constexpr int WINDOW_HEIGHT = HighQuality ?  960 : 432.0;

/*
constexpr int WINDOW_WIDTH = 256;
constexpr int WINDOW_HEIGHT = 256;
*/

constexpr int REFRACTION_WIDTH = WINDOW_WIDTH / 2;
constexpr int REFRACTION_HEIGHT = WINDOW_HEIGHT / 2;

constexpr int REFLECTION_WIDTH = WINDOW_WIDTH / 2;
constexpr int REFLECTION_HEIGHT = WINDOW_HEIGHT / 2;





const string HEIGHT_MAP_FILENAME = "heightmap.bin";
const string SPLAT_MAP_FILENAME = "splatmap.bin";
const string AO_MAP_FILENAME = "aomap.bin";

const string OBJS_FILENAME = "objs";


bool save_screenshot(string filename, int w, int h)
{
    w *= 2;
    h *= 2;


  //This prevents the images getting padded
 // when the width multiplied by 3 is not a multiple of 4
  glPixelStorei(GL_PACK_ALIGNMENT, 1);

  int nSize = w*h*3;
  // First let's create our buffer, 3 channels per Pixel
  char* dataBuffer = (char*)malloc(nSize*sizeof(char));

  if (!dataBuffer) return false;

   // Let's fetch them from the backbuffer
   // We request the pixels in GL_BGR format, thanks to Berzeger for the tip
   glReadPixels((GLint)0, (GLint)0,
		(GLint)w, (GLint)h,
		 GL_BGR, GL_UNSIGNED_BYTE, dataBuffer);

   //Now the file creation
   FILE *filePtr = fopen(filename.c_str(), "wb");
   if (!filePtr) return false;


   unsigned char TGAheader[12]={0,0,2,0,0,0,0,0,0,0,0,0};
   unsigned char header[6] = {(unsigned char) (w%256), (unsigned char) (w/256),
			      (unsigned char) (h%256), (unsigned char)( h/256),
			       24,0};
   // We write the headers
   fwrite(TGAheader,	sizeof(unsigned char),	12,	filePtr);
   fwrite(header,	sizeof(unsigned char),	6,	filePtr);
   // And finally our image data
   fwrite(dataBuffer,	sizeof(GLubyte),	nSize,	filePtr);
   fclose(filePtr);

  return true;
}


void ToClipboard(const std::string& str) {
    std::string command = "echo '" + str + "' | pbcopy";
    system(command.c_str());
}

//(0.705072, 0.0758142, 0.705072)
TuhuApplication::TuhuApplication(int argc, char *argv[]):Application(argc, argv, WINDOW_WIDTH, WINDOW_HEIGHT),
m_curCamera(NULL), m_heightMap(NULL), m_skydome(NULL), m_gui(NULL), m_lightDirection (

    Vector3f(-0.705072f, -0.958142f, -0.705072f).Normalize(),
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

    m_aabbWireframe = Cube::Load();

    m_gpuProfiler = new GpuProfiler();


    m_ssaoPass = new SsaoPass(GetFramebufferWidth(),GetFramebufferHeight());
    m_lightingPass = new LightingPass(GetFramebufferWidth(),GetFramebufferHeight());


    m_grid = new Grid();

    m_skybox = new Skybox();

    m_envFbo = new EnvFBO();
    m_envFbo->Init(ENV_FBO_TEXTURE_UNIT, 512, 512);

    LOG_I("LOG1");

    m_refractionFbo = new ColorDepthFbo();
    m_refractionFbo->Init(REFRACTION_FBO_TEXTURE_UNIT, REFRACTION_WIDTH, REFRACTION_HEIGHT);

    m_reflectionFbo = new ColorFBO();
    m_reflectionFbo->Init(REFLECTION_FBO_TEXTURE_UNIT, REFLECTION_WIDTH, REFLECTION_HEIGHT);


    m_cubeMapTexture = CubeMapTexture::Load(
	"img/bluecloud_ft.png",
	"img/bluecloud_bk.png",
	"img/bluecloud_lf.png",
	"img/bluecloud_rt.png",
	"img/bluecloud_up.png",
	"img/bluecloud_dn.png"
	);
    if(m_cubeMapTexture == NULL) {
	PrintErrorExit();
    }


    LOG_I("LOG2");

    currentObjId = 0;

    Config& m_config = Config::GetInstance();
    if(m_config.IsGui()) {

	m_gui = new Gui(m_window);
	m_gui->AddListener(this);

	GuiMouseState::Init(m_guiVerticalScale);

    } else {

	LOG_I("No GUI created");
    }

    LOG_I("LOG3");

    m_cameraFrustum = new ViewFrustum();
    m_lightFrustum = new ViewFrustum();
    m_reflectionFrustum = new ViewFrustum();

    m_totalDelta = 0;

    // NOTE: we can fix the shadows by setting trans to (0,0,0).
    Vector3f trans = Vector3f(-2,9.0,-2);
/*
    m_smoke = new SmokeEffect(Vector3f(10,3,10) + trans);
    m_smoke->Init();
*/
    ::SetDepthTest(true);
    ::SetCullFace(true);


    const Vector3f pos =
	Vector3f(19.691696, 19.842316, 19.874208);



    m_freeCamera = new Camera(
	GetFramebufferWidth(),
	GetFramebufferHeight(),
	pos,

	Vector3f(-0.662049, -0.452847, -0.597178)
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

    m_geoObjs[m_car->GetId()] = m_car;

    bool guiMode = (m_gui != 0);

    m_line = Line::Load(Vector3f(0), -1000.0f * Vector3f(m_lightDirection), Vector3f(1,0,0) );

    if(ResourceManager::GetInstance().PathExists(dir) /*&& false*/) {

	m_heightMap = new HeightMap(
	    File::AppendPaths(dir, HEIGHT_MAP_FILENAME ) ,
	    File::AppendPaths(dir, SPLAT_MAP_FILENAME ),
	    File::AppendPaths(dir, AO_MAP_FILENAME ),

	    guiMode);

	ParseObjs(File::AppendPaths(dir, OBJS_FILENAME ));

    } else {

	m_heightMap = new HeightMap(guiMode);

	LoadObj("obj/wood_floor.eob", Vector3f(-10,0,40)+ trans );

	LoadObj("obj/plane.eob", Vector3f(0,-2.5,0)+ trans);

	m_selected = NULL;

/*
	LoadObj("obj/wall.eob",
				 Vector3f(29.152159f, 13.744261f, 21.152159f)+ trans  + Vector3f(60,0,60)
	    );*/
    }

    if(m_gui) {
	m_heightMap->SetCursorSize(m_gui->GetCursorSize());

	// do a first update, in order to setup the PBOs
	m_heightMap->UpdateGui(0, NULL, 0,0);
	m_heightMap->UpdateGui(0, NULL, 0,0);

    }

    m_physicsWorld = new PhysicsWorld(m_heightMap->GetAABB() );



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


    m_gbuffer = new Gbuffer();

    // TODO: should not this be the size of the framebuffer?
    m_gbuffer->Init(0, GetFramebufferWidth(),GetFramebufferHeight() );

    /*
      OpenAL::Initp();

      m_windSound = new Sound("audio/wind2.wav");
      m_windSound->SetGain(1.0f);
      m_windSound->SetLooping(true);
    */

    m_curCamera = m_freeCamera;

    LOG_I("LOG5");


//    StartPhysics();

    LightUpdate();
}


Matrix4f TuhuApplication::MakeLightProj(int frameBufferWidth, int frameBufferHeight)const {

    // TODO: does not yet fully work.
    //http://www.gamedev.net/topic/505893-orthographic-projection-for-shadow-mapping/
    // ALSO, note that the size of the frame buffer is a square, but the frame buffer is not!
    // this may have to be taken into account.

    // Also see "Practical Shadow Mapping"
    // for linear z-values.


    /*
      First we compute the world space location of all 8 corners of the view frustum.
    */

    Matrix4f invProj = m_curCamera->GetVp().Inverse();

    const float zNear =  -1.0f;
    const float zFar  = 1.0f;

    // left bottom far
    Vector4f temp = Vector4f(-1,-1,zFar,1.0f);
    // LOG_I("temp: %f", temp.w );
    const Vector3f lbf = m_cameraFrustum->lbf();

/*
  LOG_I("sanity: %s",
  string(m_camera->GetMvpFromM() * Vector4f(17.328205, 15.360136, 14.091190, 1.0)).c_str()

  );

  exit(1);*/

//    	Vector3f(17.328205, 15.360136, 14.091190);


    // left top far
    const Vector3f ltf = m_cameraFrustum->ltf();

    // right bottom far
    const Vector3f rbf = m_cameraFrustum->rbf();

    // right top far
    const Vector3f rtf = m_cameraFrustum->rtf();

    // left bottom near
    const Vector3f lbn = m_cameraFrustum->lbn();

    // left top near
    const Vector3f ltn = m_cameraFrustum->ltn();


    // right bottom far
    const Vector3f rbn = m_cameraFrustum->rbn();

    // right top far
    const Vector3f rtn = m_cameraFrustum->rtn();
/*
    LOG_I("lbf: %s",  string(lbf).c_str() );

    LOG_I("ltf: %s",  string(ltf).c_str() );
    LOG_I("rbf: %s",  string(rbf).c_str() );
    LOG_I("rtf: %s",  string(rtf).c_str() );
    LOG_I("lbn: %s",  string(lbn).c_str() );
    LOG_I("ltn: %s",  string(ltn).c_str() );
    LOG_I("rbn: %s",  string(rbn).c_str() );
    LOG_I("rtn: %s",  string(rtn).c_str() );
*/
    Vector3f corners[8] =  {lbn , ltn , rbn , rtn ,
			    lbf , ltf , rbf , rtf };

    Vector3f centroid = Vector3f(0,0,0);
    for(int i = 0; i < 8; ++i) {
	centroid += corners[i];
    }
    centroid = centroid * (1.0f/8.0f);


    Config& config = Config::GetInstance();

    const float nearClipOffset = 70.0f;


    Matrix4f viewMatrix =
	Matrix4f::CreateLookAt(
	    centroid - ( Vector3f(m_lightDirection) * (config.GetZFar() + nearClipOffset )   ),
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
  LOG_I("min x %f",  mins.x );
  LOG_I("max x %f",  maxes.x  );

  LOG_I("min y %f",  mins.y );
  LOG_I("max y %f",  maxes.y  );

  LOG_I("%f", -maxes.z - nearClipOffset  );
  LOG_I("%f",  -mins.z );
*/


/*    mins.x = mins.y = -300;
    maxes.x = maxes.y = 300;
/*
    float fWorldUnitsPerTexel = (float)(maxes.x - mins.x)/(float)(SHADOW_MAP_SIZE);

    //  LOG_I("before min.x:%f",  mins.x);


    mins.x /= fWorldUnitsPerTexel;
    mins.x = floor(mins.x);
    mins.x *= fWorldUnitsPerTexel;

    //LOG_I("after min.x:%f",  mins.x);


    maxes.x /= fWorldUnitsPerTexel;
    maxes.x = floor(maxes.x);
    maxes.x *= fWorldUnitsPerTexel;


    fWorldUnitsPerTexel = (float)(maxes.y - mins.y)/(float)(SHADOW_MAP_SIZE);

    mins.y /= fWorldUnitsPerTexel;
    mins.y = floor(mins.y);
    mins.y *= fWorldUnitsPerTexel;

    maxes.y /= fWorldUnitsPerTexel;
    maxes.y = floor(maxes.y);
    maxes.y *= fWorldUnitsPerTexel;
*/
    /*
      How to fix flickering:
      http://www.gamedev.net/topic/591684-xna-40---shimmering-shadow-maps/
      https://msdn.microsoft.com/en-us/library/ee416324(v=vs.85).aspx
      http://www.gamedev.net/topic/645795-shimmering-shadow-edges/

      http://www.gamedev.net/topic/669082-snap-cameraprojection-to-pixel-increments/
      https://github.com/ehsan/ogre/blob/master/OgreMain/src/OgreShadowCameraSetup.cpp
     */


    // TODO: the problem probably lies in that we should flip maxz and minz or something.
    return Matrix4f::CreateOrthographic(mins.x, maxes.x,

					mins.y,
					maxes.y,

					-maxes.z - nearClipOffset,

					-mins.z ) * viewMatrix;
}

void TuhuApplication::RenderShadowMap() {

    m_depthFbo->Bind();
    {
	::SetViewport(0,0,SHADOW_MAP_SIZE,SHADOW_MAP_SIZE);

	Clear(0.0f, 1.0f, 1.0f, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GeometryObject::RenderShadowMapAll(m_lightVp);

	m_heightMap->RenderShadowMap(m_lightVp);
    }
     m_depthFbo->Unbind();

}

void TuhuApplication::RenderId() {

    m_pickingFbo->Bind();

    Clear(0.0f, 0.0f, 0.0f, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // set viewport
//    m_heightMap->RenderId(m_curCamera);

    GeometryObject::RenderIdAll(m_curCamera);

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

    Matrix4f biasMatrix(
	0.5f, 0.0f, 0.0f, 0.5f,
	0.0f, 0.5f, 0.0f, 0.5f,
	0.0f, 0.0f, 0.5f, 0.5f,
	0.0f, 0.0f, 0.0f, 1.0f
	);

    Matrix4f lightVp =  biasMatrix*   m_lightVp;







    m_gpuProfiler->Begin(GTS_Objects);
    {
	GeometryObject::RenderAll(m_curCamera, m_lightDirection, lightVp, *m_depthFbo, m_envFbo->GetEnvMap(), *m_refractionFbo, *m_reflectionFbo);
    }

//    m_smoke->Render(m_curCamera->GetVp(), m_curCamera->GetPosition());

//    m_line->Render(m_curCamera->GetVp());

    m_gpuProfiler->End(GTS_Objects);




    m_gpuProfiler->Begin(GTS_Terrain);



    bool aoOnly = m_gui ? m_gui->isAoOnly() : false;

    m_heightMap->Render(m_curCamera, m_lightDirection, lightVp, *m_depthFbo, aoOnly);

    m_gpuProfiler->End(GTS_Terrain);





//    m_grass->Draw(m_curCamera, m_lightDirection);

    //  m_snow->Render(m_curCamera->GetMvpFromM(), m_curCamera->GetPosition());



}

void TuhuApplication::RenderEnvMap() {

    for(int i = 0; i < 6; ++i) {

	if(i == 3) // dont draw for bottom cube map side.
	    continue;

	// bind fbo
	m_envFbo->Bind();
	{

	    m_envFbo->BindFace(i);

	    size_t size = m_envFbo->GetSize();
	    ::SetViewport(0,0,size,size);

	    Clear(1.0f, 1.0f, 1.0f, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	    //   m_skydome->Draw(m_car->GetEnvCameras()[i]);

	    m_skybox->DrawForward(m_cubeMapTexture, m_car->GetEnvCameras()[i]);

	    GeometryObject::RenderAllEnv(m_car->GetEnvCameras()[i], m_lightDirection, i);

	    bool aoOnly = m_gui ? m_gui->isAoOnly() : false;
	    m_heightMap->RenderEnvMap(m_car->GetEnvCameras()[i], m_lightDirection, i, aoOnly);
	}
	m_envFbo->Unbind();
    }
}

void TuhuApplication::RenderRefraction() {

    m_refractionFbo->Bind();
    {
	::SetViewport(0,0,REFRACTION_WIDTH, REFRACTION_HEIGHT);
	Clear(0.0f, 1.0f, 1.0f, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	bool aoOnly = m_gui ? m_gui->isAoOnly() : false;
	m_heightMap->RenderRefraction(m_curCamera, m_lightDirection, aoOnly);
    }
    m_refractionFbo->Unbind();
}

void TuhuApplication::RenderReflection() {

    m_reflectionFbo->Bind();
    {
	::SetViewport(0,0,REFLECTION_WIDTH, REFLECTION_HEIGHT);
	Clear(0.0f, 1.0f, 1.0f, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_skybox->DrawForward(m_cubeMapTexture, m_reflectionCamera );

	GeometryObject::RenderReflection(m_reflectionCamera, m_lightDirection);

	bool aoOnly = m_gui ? m_gui->isAoOnly() : false;
	m_heightMap->RenderReflection(m_reflectionCamera, m_lightDirection, aoOnly);


    }
    m_reflectionFbo->Unbind();
}

void TuhuApplication::Render() {


    if(m_gui) {
	m_gui->NewFrame(m_guiVerticalScale);
    }

    m_gpuProfiler->Begin(GTS_Shadows);
    RenderShadowMap();
    m_gpuProfiler->End(GTS_Shadows);

    m_gpuProfiler->Begin(GTS_EnvMap);
    RenderEnvMap();
    m_gpuProfiler->End(GTS_EnvMap);

    m_gpuProfiler->Begin(GTS_Refraction);
    RenderRefraction();
    m_gpuProfiler->End(GTS_Refraction);

    m_gpuProfiler->Begin(GTS_Reflection);
    RenderReflection();
    m_gpuProfiler->End(GTS_Reflection);



    float SCALE = m_guiVerticalScale;

    // set the viewport to the size of the window.

    int windowWidth;
    int windowHeight;

    SetViewport();
    Clear(0.0f, 1.0f, 1.0f, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if(m_pickingFbo)
	RenderId();

    m_gbuffer->BindForWriting();

    SetViewport();
    Clear(0.0f, 1.0f, 1.0f, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    RenderScene();

   m_gbuffer->UnbindForWriting();

	// m_aabbWireframe->Render(m_curCamera->GetVp());


//    m_grid->Draw();

    Matrix4f biasMatrix(
	0.5f, 0.0f, 0.0f, 0.5f,
	0.0f, 0.5f, 0.0f, 0.5f,
	0.0f, 0.0f, 0.5f, 0.5f,
	0.0f, 0.0f, 0.0f, 1.0f
	);

    Matrix4f lightVp =  biasMatrix*   m_lightVp;

    m_gpuProfiler->Begin(GTS_Light);
    m_lightingPass->Render(
	m_gbuffer, m_curCamera, m_lightDirection,
	lightVp, *m_depthFbo, GeometryObject::GetTorches(),
	m_envFbo->GetEnvMap(), *m_refractionFbo, *m_reflectionFbo,
    *m_cameraFrustum
	);
    m_gpuProfiler->End(GTS_Light);

//    m_smoke->Render(m_curCamera->GetVp(), m_curCamera->GetPosition());


    m_gpuProfiler->Begin(GTS_Sky);
    m_skybox->DrawDeferred(
//	m_cubeMapTexture,
	m_envFbo->GetEnvMap(),
	m_curCamera, m_gbuffer->GetDepthTexture(), GetFramebufferWidth(), GetFramebufferHeight() );
    m_gpuProfiler->End(GTS_Sky);


    m_fire->Render(m_curCamera->GetVp(), m_curCamera->GetPosition());

    if(m_gui) {

	int fb_width, fb_height;
	fb_width = GetFramebufferWidth();
	fb_height = GetFramebufferHeight();

 	windowWidth = fb_width*SCALE * 0.5f;
	windowHeight = fb_height * 0.5f;


	m_gui->Render(windowWidth, windowHeight);
    }


    m_gpuProfiler->WaitForDataAndUpdate();

    m_gpuProfiler->EndFrame();

}

void TuhuApplication::Update(const float delta) {


    MouseState& ms = MouseState::GetInstance();

    m_totalDelta += delta;

    m_curCamera->Update(delta);
    m_reflectionCamera = m_curCamera->CreateReflectionCamera();

    GeometryObject::SetTotalDelta(m_totalDelta);


    m_fire->Update(delta);

    if(m_gui)
	GuiMouseState::Update(GetFramebufferWidth(), GetFramebufferHeight());

    m_cameraFrustum->Update( m_curCamera->GetVp(), m_curCamera->GetPosition() );
    m_reflectionFrustum->Update( m_reflectionCamera->GetVp(),  m_curCamera->GetPosition()  );

    UpdateMatrices();

    m_lightFrustum->Update( m_lightVp, Vector3f(0) );

    m_physicsWorld->Update(delta);

    for(auto& it : m_geoObjs) {
//    for(IGeometryObject* geoObj: m_geoObjs) {
	IGeometryObject* geoObj = it.second;

	geoObj->Update(m_cameraFrustum, m_lightFrustum, m_car->GetLightFrustums(), m_reflectionFrustum );


	//update cameras of env map. here
    }

    m_heightMap->Update(*m_cameraFrustum, *m_lightFrustum, m_car->GetLightFrustums(), *m_reflectionFrustum );


//    m_smoke->Update(delta);

    if(m_gui) {
	m_gui->Update();
	m_heightMap->SetGuiMode( m_gui->GetGuiMode() );
    }

    KeyboardState& kbs = KeyboardState::GetInstance();

    if( kbs.IsPressed(GLFW_KEY_P) ) {

	string out = "Vector3f" +tos(m_curCamera->GetPosition()) + ",";
	out += "Vector3f" + tos(m_curCamera->GetViewDir());
	ToClipboard(out);
    }

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
	} else if(m_gui->GetGuiMode() == RoadMode) {

	    if(ms.WasPressed(GLFW_MOUSE_BUTTON_1 )) {
		m_heightMap->AddControlPoint();
	    }
	}



    }

    if(ms.WasPressed(GLFW_MOUSE_BUTTON_1 ) && GuiMouseState::isWithinWindow()) {

	if(m_gui->GetGuiMode() == ModelMode) {

	    float y = GetFramebufferHeight() - GuiMouseState::GetY() - 1;
	    float x = GuiMouseState::GetX();

	    PixelInfo pi = m_pickingFbo->ReadPixel((unsigned int)x,(unsigned int)y);

	    unsigned int id = (unsigned int)pi.id;

	    if(id != 0) {

		// id of 0 is no object picked at all
		// however, the ids in the map are 0-based.
		// thus decrease by one.
		id -= 1;

		if(m_selected){
		    // deselected formerly selected object.
		    m_selected->SetSelected(false);
		}

		LOG_I("selected obj: %d", id);

		m_selected = m_geoObjs[id];
		m_selected->SetSelected(true);

	    }


	}


    }


    if(m_gui && m_gui->GetGuiMode() == ModelMode && m_selected ) {

	m_selected->SetEditPosition( m_gui->GetTranslation() );
	m_selected->SetEditRotation( toBtQuat(m_gui->GetRotation()) );
	m_selected->SetEditScale( m_gui->GetScale());

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
/*
    if( kbs.IsPressed(GLFW_KEY_L) ) {
	save_screenshot("screen.tga", WINDOW_WIDTH, WINDOW_HEIGHT);
    }
*/

    //
    m_heightMap->UpdateGui(delta, m_curCamera, (float)GetFramebufferWidth(),(float)GetFramebufferHeight());

}

string Format(const string& fmt, float val) {

    char buffer[30];

    sprintf(buffer,
	    fmt.c_str(),
	    val);

    return string(buffer);
}

void TuhuApplication::RenderText()  {

    string cull = std::to_string(nonCulledObjects) + "\\" + std::to_string(totalObjects);

    m_font->DrawString(*m_fontShader, 750,170, cull);

    m_font->DrawString(*m_fontShader, 750,120, tos(m_curCamera->GetPosition())  );

    m_font->DrawString(*m_fontShader, 750,220,
		       Format("Objects: %0.2f ms", m_gpuProfiler->DtAvg(GTS_Objects) ) );

    m_font->DrawString(*m_fontShader, 750,270,
		       Format("Sky: %0.2f ms", m_gpuProfiler->DtAvg(GTS_Sky) ) );

    m_font->DrawString(*m_fontShader, 750,320,
		       Format("Terrain: %0.2f ms", m_gpuProfiler->DtAvg(GTS_Terrain) ) );

    m_font->DrawString(*m_fontShader, 750,370,
		       Format("Shadows: %0.2f ms", m_gpuProfiler->DtAvg(GTS_Shadows) ) );

    m_font->DrawString(*m_fontShader, 750,430,
		       Format("Light<x: %0.2f ms", m_gpuProfiler->DtAvg(GTS_Light) ) );

    m_font->DrawString(*m_fontShader, 750,490,
		       Format("EnvMap: %0.2f ms", m_gpuProfiler->DtAvg(GTS_EnvMap) ) );

    m_font->DrawString(*m_fontShader, 750,550,
		       Format("Refraction: %0.2f ms", m_gpuProfiler->DtAvg(GTS_Refraction)) );

    m_font->DrawString(*m_fontShader, 750,610,
		       Format("Reflection: %0.2f ms", m_gpuProfiler->DtAvg(GTS_Reflection)) );



}

IGeometryObject* TuhuApplication::LoadObj(const std::string& path, const Vector3f& position,
					  const btQuaternion& rotation, float scale) {


    GeometryObject* obj = new GeometryObject();

//    LOG_I("add model id: %d", currentObjId);
    bool result = obj->Init(path, position,rotation, scale, currentObjId++, COL_STATIC, staticCollidesWith);

    if(!result)
	PrintErrorExit();

    m_geoObjs[obj->GetId()] = obj;

    return obj;
}

void TuhuApplication::StartPhysics()  {

    static bool once = false;

    if(once)
	return;

    for(auto& it : m_geoObjs) {
	IGeometryObject* geoObj = it.second;

	if(geoObj == m_car || geoObj->GetFilename() == string("obj/plane.eob"))
	    geoObj->AddToPhysicsWorld(m_physicsWorld);
    }
    m_heightMap->AddToPhysicsWorld(m_physicsWorld);

    once = true;

}

void TuhuApplication::Cleanup() {

    string dir = Config::GetInstance().GetWorldFilename();



    if(m_gui) {
	// if we were in the world editor, we need to serialize the world.


	// we save the entire world in a directory. Make sure the directory exists:
	File::CreatePath(dir);

	// save heightmap:
	m_heightMap->SaveHeightMap(File::AppendPaths(dir, HEIGHT_MAP_FILENAME ) );

	// save splatmap:
	m_heightMap->SaveSplatMap(File::AppendPaths(dir, SPLAT_MAP_FILENAME ) );

	m_heightMap->SaveAoMap(File::AppendPaths(dir, AO_MAP_FILENAME ) );


	File* outFile = File::Load(
	    File::AppendPaths(dir, OBJS_FILENAME ),
	    FileModeWriting);

	// -1, since we do not count the car. m_car
	outFile->WriteLine("numObjs " + to_string(m_geoObjs.size()-1) );


	for(auto& it : m_geoObjs) {

	    IGeometryObject* geoObj = it.second;

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


		float s = geoObj->GetScale();
		outFile->WriteLine("scale " +
				   to_string(s )

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

	// read translation.
	vector<string> tokens = StringUtil::SplitString(reader->ReadLine(), " ");
	Vector3f translation = Vector3f(stof(tokens[1]),stof(tokens[2]), stof(tokens[3]) );

	// read rotation
	tokens = StringUtil::SplitString(reader->ReadLine(), " ");
	btQuaternion rotation = btQuaternion(
	    stof(tokens[1]),
	    stof(tokens[2]),
	    stof(tokens[3]),
	    stof(tokens[4]));

	// read scale.
	tokens = StringUtil::SplitString(reader->ReadLine(), " ");
	float scale = stof(tokens[1]);

	reader->ReadLine(); // endObj

	// parsed the object. now add it to the world.
	LoadObj(filename, translation, rotation, scale);

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

void TuhuApplication::ScaleAccepted() {
    m_selected->SetScale(

	m_selected->GetScale() * m_gui->GetScale());

    m_selected->SetEditScale( 1.0f  );
}

void TuhuApplication::ModelAdded(const std::string& filename) {

    if(m_selected)
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


	LOG_I("duplate %d, creating %d", m_selected->GetId(), dupObj->GetId() );

	// select duplicated object:
	m_selected->SetSelected(false);
	dupObj->SetSelected(true);
	m_selected = dupObj;

	m_geoObjs[dupObj->GetId()] = dupObj;
    }
}

void TuhuApplication::Delete() {
    LOG_I("delete");

    if(m_selected) {

	// not selected anymore.
	m_selected->SetSelected(false);

	LOG_I("delete %d", m_selected->GetId() );


	// delete from map:
	auto it = m_geoObjs.find(m_selected->GetId());
	GeometryObject::Delete(it->second );
	m_geoObjs.erase(it);

	m_selected = NULL;
    }
}

void TuhuApplication::UpdateMatrices() {


	Vector3f carPos = m_car->GetPosition();

	Vector3f carSide = Vector3f::Cross(m_car->GetForwardVector(), Vector3f(0,1,0));

	Vector3f cameraPos = carPos + Vector3f(m_lightDirection);

	Matrix4f lightViewMatrix = Matrix4f::CreateLookAt(
	    cameraPos,
	    carPos,
	    Vector3f(0.0, 1.0, 0.0)
	    );

/*
	m_lightViewMatrix = Matrix4f::CreateLookAt(
       	    -Vector3f(m_lightDirection),
	    Vector3f(0.0f, 0.0f, 0.0f),
	    Vector3f(0.0, 1.0, 0.0)
	    );
*/

	Config& config = Config::GetInstance();

	/*
	float left =-100;
	float right =160;
	float bottom =-60;
	float top =60;
	float near =-300;
	float far =30;
*/

	float left =-340;
	float right = 345;
	float bottom = -249;
	float top = 150;
	float near = -300;
	float far = 500;

	Matrix4f lightProjectionMatrix =  //MakeLightProj();

	    Matrix4f::CreateOrthographic(
		left,right, // left, right
		bottom, top, // bottom, top
		near, // near(front of car)
		far); // far(behind car)
//	    Matrix4f::CreateOrthographic(-350,350, -200, 200, -200, 500);

	m_lightVp = lightProjectionMatrix * lightViewMatrix;
//	LOG_I("version1 %s", string(m_lightVp).c_str() );


	Vector3f min = Vector3f(left, bottom, near) + cameraPos;
	Vector3f max = Vector3f(right, top, far) + cameraPos;

	Vector3f center = (min + max) * 0.5f;

	Vector3f radius = max - center;

	m_aabbWireframe->SetModelMatrix(
//	    lightViewMatrix *

	    Matrix4f::CreateTranslation(center ) *

	    Matrix4f::CreateScale(radius )
	   );

//	exit(1);
	m_lightVp = MakeLightProj(GetFramebufferWidth(), GetFramebufferHeight() );

//	LOG_I("version2 %s", string( m_lightVp  ).c_str() );

//	exit(1);

}

void TuhuApplication::BakeAo(int samples, int waveLength, int amplitude, float distAttenuation) {
    m_heightMap->BakeAo(samples, waveLength, amplitude, distAttenuation);
}

void TuhuApplication::LightUpdate() {
    // update textures.
    m_lightingPass->UpdateTextures(GeometryObject::GetTorches().size() );
//    m_lightingPass->UpdateTextures(-1 );
}


void TuhuApplication::BuildRoad() {
    m_heightMap->BuildRoad();
}


void TuhuApplication::DeleteCP() {

    m_heightMap->DeleteCP();

}
