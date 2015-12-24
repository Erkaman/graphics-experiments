#include "tuhu_application.hpp"

#include "ewa/camera.hpp"
#include "ewa/common.hpp"
#include "ewa/font.hpp"
#include "ewa/keyboard_state.hpp"
#include "ewa/mouse_state.hpp"

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

#include "gui.hpp"
#include "gui_mouse_state.hpp"

#include "nfd.h"

using namespace std;

int nonCulledObjects = 0;
int totalObjects = 0;

constexpr int SHADOW_MAP_SIZE = 1024;

void ToClipboard(const std::string& str) {
    std::string command = "echo '" + str + "' | pbcopy";
    system(command.c_str());
}

//(0.705072, 0.0758142, 0.705072)
TuhuApplication::TuhuApplication(int argc, char *argv[]):Application(argc, argv), m_curCamera(NULL), m_heightMap(NULL), m_skydome(NULL), m_gui(NULL), m_lightDirection (

    -0.705072f, -0.458142f, -0.705072f,
//    -0.705072f, -0.0758142f, -0.705072f ,

    0.0f)   { }

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

    Config& m_config = Config::GetInstance();
    if(m_config.IsGui()) {

	m_gui = new Gui(m_window);

	GuiMouseState::Init(m_guiVerticalScale);

    } else {

	LOG_I("No GUI created");
    }


    m_physicsWorld = new PhysicsWorld();

    m_viewFrustum = new ViewFrustum();

    m_totalDelta = 0;

    // NOTE: we can fix the shadows by setting trans to (0,0,0).
    Vector3f trans = Vector3f(-80,-1,-80);

  m_smoke = new SmokeEffect(Vector3f(10,3,10) + trans);
    m_smoke->Init();

    ::SetDepthTest(true);
    ::SetCullFace(true);

    const Vector3f pos =
	Vector3f(51.479908, 40.918278, 70.826126);

    m_freeCamera = new Camera(
	GetFramebufferWidth(),
	GetFramebufferHeight(),
			  pos,
	Vector3f(-0.613098, -0.523130, -0.591984)
			  );

    m_snow = new SnowEffect(pos);
    m_snow->Init();



    m_fire = new FireEffect(Vector3f(12,-3,10));
    m_fire->Init();

   m_heightMap = new HeightMap("img/hill.png");
   m_heightMap->AddToPhysicsWorld(m_physicsWorld);

    //                    128000
    m_skydome = new Skydome(1, 10, 10);

//   m_grass = new Grass(Vector2f(10,10), m_heightMap);

//   LOG_I("now we start creating objs");

   m_stoneFloor = LoadObj("obj/rock_floor.eob", Vector3f(0,0,40) + trans);


//   LOG_I("created one single obj");

   m_flatWoodFloor = LoadObj("obj/flat_wood_floor.eob", Vector3f(10,0,40)+ trans );

   m_woodFloor = LoadObj("obj/wood_floor.eob", Vector3f(-10,0,40)+ trans );

   m_sphere = LoadObj("obj/sunball.eob", Vector3f(21.152159f, 13.744261f, 21.152159f)+ trans);

//   LOG_I("created some objs");


   m_plane = LoadObj("obj/plane.eob", Vector3f(0,-2.5,0)+ trans);

   m_tree = LoadObj("obj/tree.eob", Vector3f(10,-2.5,10) + trans);

//   LOG_I("created some more objs");

   m_wall = LoadObj("obj/wall.eob", Vector3f(-5,-2.5,-5)  + trans);

   m_wall2 = LoadObj("obj/wall.eob", Vector3f(20,-6.5,-5) + trans);

   m_car = Car::Load(m_physicsWorld, Vector3f(0,-1.5,0)+trans);
    if(!m_car)
	PrintErrorExit();

    m_geoObjs.push_back(m_car);


//   m_ball2 = LoadObj("obj/sunball.eob", Vector3f(20,-1.0,-7) );

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

/*
  m_stoneFloor->RenderShadowMap(vp);

  m_flatWoodFloor->RenderShadowMap(vp);

  m_woodFloor->RenderShadowMap(vp);
*/
	m_sphere->RenderShadowMap(vp);

	m_tree->RenderShadowMap(vp);

	m_wall->RenderShadowMap(vp);
	m_wall2->RenderShadowMap(vp);

//	m_ball2->RenderShadowMap(vp);

    }
    m_depthFbo->Unbind();
}

void TuhuApplication::RenderId() {

    m_pickingFbo->Bind();

    Clear(0.0f, 0.0f, 0.0f, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // set viewport
    m_heightMap->RenderId(m_curCamera);

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


    nonCulledObjects = 0;
    for(IGeometryObject* geoObj: m_geoObjs) {

	if(m_viewFrustum->IsAABBInFrustum(geoObj->GetModelSpaceAABB())) {
	    ++nonCulledObjects;
	   geoObj->Render(m_curCamera, m_lightDirection, lightVp, *m_depthFbo);
	}

    }

    totalObjects = m_geoObjs.size();

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


/*   if(m_pickingFbo)
	RenderId();
*/
   RenderScene();


    if(m_gui) {

	int fb_width, fb_height;
	fb_width = GetFramebufferWidth();
	fb_height = GetFramebufferHeight();

 	windowWidth = fb_width*SCALE * 0.5;
	windowHeight = fb_height * 0.5;


	m_gui->Render(windowWidth, windowHeight);
    }



}

void TuhuApplication::Update(const float delta) {

    MouseState& ms = MouseState::GetInstance();

    GuiMouseState::Update(GetFramebufferWidth(), GetFramebufferHeight());


    m_heightMap->Update(delta, m_curCamera, GetFramebufferWidth(),GetFramebufferHeight());

    m_viewFrustum->Update( m_curCamera->GetVp() );

    m_physicsWorld->Update(delta);

    m_car->Update();

    m_totalDelta += delta;


    m_curCamera->Update(delta);

    m_smoke->Update(delta);
/*    //   m_snow->Update(delta);
    m_fire->Update(delta);
	*/
    m_skydome->Update(delta);

//      m_grass->Update(delta);

    KeyboardState& kbs = KeyboardState::GetInstance();

    if( kbs.IsPressed(GLFW_KEY_P) ) {

	string out = "Vector3f" +tos(m_curCamera->GetPosition()) + ",";
	out += "Vector3f" + tos(m_curCamera->GetViewDir());
	ToClipboard(out);
    }


    if( kbs.IsPressed(GLFW_KEY_7) ) {
	m_heightMap->SaveHeightMap();
    }

    if( kbs.IsPressed(GLFW_KEY_8) ) {
	m_heightMap->SaveSplatMap();
    }

    if( kbs.WasPressed(GLFW_KEY_9) ) {

	nfdchar_t *outPath = NULL;

	nfdresult_t result = NFD_OpenDialog( "png,jpg;pdf", NULL, &outPath );

	if ( result == NFD_OKAY ) {
	    puts("Success!");
	    puts(outPath);
	    free(outPath);
	}
    }

    if(ms.IsPressed(GLFW_MOUSE_BUTTON_1 )) {

	if(GuiMouseState::isWithinWindow()) {
	    // remap the mouse position to the game framebuffer:


	    if(m_gui->GetGuiMode() == ModifyTerrainMode) {

		m_heightMap->ModifyTerrain(delta);

	    } else {
		m_heightMap->DrawTexture(delta, m_gui->GetDrawTextureType() );

	    }


/*		float y = GetFramebufferHeight() - GuiMouseState::GetY() - 1;
		float x = GuiMouseState::GetX();

//		LOG_I("mouse: %f, %f", x, y);

PixelInfo pi = m_pickingFbo->ReadPixel(x,y);
*/
//		LOG_I("triangle: %f", pi.unused1);
	} else {
//	    LOG_I("IGNORE");
	}

/*
  if(m_pickingFbo) {

  }
*/
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
    }
}

void TuhuApplication::RenderText()  {

    string cull = std::to_string(nonCulledObjects) + "\\" + std::to_string(totalObjects);

    m_font->DrawString(*m_fontShader, 750,170, cull);

    m_font->DrawString(*m_fontShader, 750,120, tos(m_curCamera->GetPosition())  );



//    m_font->DrawString(*m_fontShader, 600,120, cull );


}

IGeometryObject* TuhuApplication::LoadObj(const std::string& path, const Vector3f& position) {

    IGeometryObject* obj = GeometryObject::Load(path, position, m_physicsWorld);

    if(!obj)
	PrintErrorExit();

    m_geoObjs.push_back(obj);

    return obj;
}
