#include "grass_application.hpp"

#include "ewa/camera.hpp"
#include "ewa/font.hpp"
#include "ewa/keyboard_state.hpp"
#include "ewa/quad.hpp"

#include "ewa/gl/fbo.hpp"
#include "ewa/gl/texture.hpp"
#include "ewa/gl/render_buffer.hpp"
#include "ewa/gl/shader_program.hpp"

#include "ewa/math/vector4f.hpp"

#include "plane.hpp"
#include "grass.hpp"
#include "grasses.hpp"

#include <stdlib.h>

using namespace std;

constexpr bool CAMERA = true;
constexpr int IMAGE_WIDTH = 512;
constexpr int IMAGE_HEIGHT = 512;


void ToClipboard(const std::string& str) {
    std::string command = "echo '" + str + "' | pbcopy";
    system(command.c_str());
}

GrassApplication::GrassApplication(int argc, char *argv[]):Application(argc, argv, 512, 512), m_camera(NULL), m_plane(NULL) { }

GrassApplication::~GrassApplication() {
    MY_DELETE(m_camera);
    MY_DELETE(m_plane);
}

void GrassApplication::Init() {

    ::SetDepthTest(true);
    ::SetCullFace(true);

    m_camera = new Camera(GetWindowWidth(),GetWindowHeight(),

			  Vector3f(6.797705, 4.315825, 3.393999),Vector3f(0.113218, 0.175353, 0.977974)



		       , true);

    m_plane = new Plane(Vector3f(5,4,1), Vector3f(1,1,1));

    m_grass = new Exp();
    m_grass->Init();

    if(CAMERA) {
    WIDTH = GetFramebufferWidth();
    HEIGHT = GetFramebufferHeight();

    } else {
	WIDTH = IMAGE_WIDTH;
	HEIGHT = IMAGE_HEIGHT;

    }



    m_fbo1 = new FBO(9,
		     WIDTH,HEIGHT);


    m_fbo2 = new FBO(9,
			      WIDTH,HEIGHT);


    m_quad = new Quad(Vector2f(-1.0f), Vector2f(1.0f));

    m_postShader = new ShaderProgram("shader/post");


/*
    GLint samples;
    glGetIntegerv(GL_MAX_SAMPLES, &samples);

    LOG_I("samples: %d", samples);*/

    LOG_I("width: %d", GetFramebufferWidth());

}

void GrassApplication::Render() {

    m_fbo1->Bind();
    {

	::SetViewport(0, 0, WIDTH, WIDTH);
	GL_C(glClearColor(0, 0, 0, 0.0f));
	GL_C(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));


	m_grass->Draw(*m_camera);
    }
    m_fbo1->Unbind();
/*    m_fbo1->GetRenderTargetTexture().WriteToFile("grass.png");
      exit(1);*/

    if(!CAMERA) {
	m_fbo2->Bind();
    }
    {
	::SetViewport(0, 0, WIDTH, WIDTH);
	GL_C(glClearColor(0, 0, 0, 0.0f));
	GL_C(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));


	m_postShader->Bind();

	Texture::SetActiveTextureUnit(m_fbo1->GetTargetTextureUnit());

	m_postShader->SetUniform("tex", (int)m_fbo1->GetTargetTextureUnit() );

	m_fbo1->GetRenderTargetTexture().Bind();

	m_quad->Draw();

	m_fbo1->GetRenderTargetTexture().Unbind();

	m_postShader->Unbind();
    }
    if(!CAMERA) {
	m_fbo2->Unbind();

	m_fbo2->GetRenderTargetTexture().WriteToFile("grass.png");
	exit(1);
    }



}

void GrassApplication::Update(const float delta) {
    m_camera->HandleInput(delta);
    m_grass->Update(delta);

    const KeyboardState& kbs = KeyboardState::GetInstance();

    if( kbs.IsPressed(GLFW_KEY_P) ) {


	string out = "Vector3f" +tos(m_camera->GetPosition()) + ",";
	out += "Vector3f" + tos(m_camera->GetViewDir());
	ToClipboard(out);

    }
}

void GrassApplication::RenderText()  {
//    m_font->DrawString(*m_fontShader, 600,150, "hello world" );

//    m_font->DrawString(*m_fontShader, 100,150, tos(m_camera->GetPosition()).c_str() );
//    m_font->DrawString(*m_fontShader, 100,250, tos(m_camera->GetViewDir()).c_str() );
//Vector3f(6.884536, 4.450306, 4.144031),Vector3f(0.113218, 0.175353, 0.977974)

}
