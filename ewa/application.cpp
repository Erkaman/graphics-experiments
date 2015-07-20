#include "application.hpp"

#include "gl/gl_common.hpp"
#include "gl/vao.hpp"
#include "gl/shader_program.hpp"
#include "gl/texture.hpp"
#include "gl/vbo.hpp"

#include "font.hpp"
#include "log.hpp"
#include "keyboard_state.hpp"

#include <stdio.h>
#include <stdlib.h>

#include "fps_manager.hpp"
#include "mouse.hpp"


using namespace std;

void Application::Start() {

    LogInit();

    this->SetupOpenGL();

    m_fontShader = new ShaderProgram("shader/font_render");

    m_font = new Font("img/font.png",
			     624,624,
			     39,39,
			     GetWindowWidth(),GetWindowHeight());


    KeyboardState::GetInstance().Init(m_window);

    this->Init();
    this->DoMainLoop();
    this->Cleanup();
}

void Application::DoMainLoop() {
    m_running = true;

    FPSManager fps;

    fps.Start();

    float delta = 0.0f;


    while(m_running) {

	this->Update_internal(delta);

	this->Render();

	RenderText_internal(fps.GetFpsString());



	// update frame buffer
	glfwSwapBuffers(m_window);
	// update input.
	glfwPollEvents();

	delta = fps.ManageFPS();

//	SetWindowTitle(fps.GetFpsString());

//	std::string str = camera

    }
}

void Application::Cleanup() {
    m_vao->Unbind();
    delete m_vao;
    delete m_fontShader;
    delete m_font;

    glfwTerminate();
    LogDispose();
}


void Application::SetupOpenGL() {

    // start GL context and O/S window using the GLFW helper library
    if (!glfwInit ()) {
	fprintf (stderr, "ERROR: could not start GLFW3\n");
	exit(1);
    }

    // uncomment these lines if on Apple OS X
    glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint (GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

    m_window = glfwCreateWindow (800, 600, "Tuhu", NULL, NULL);
    if (!m_window) {
	fprintf (stderr, "ERROR: could not open window with GLFW3\n");
	glfwTerminate();
	exit(1);
    }
    glfwMakeContextCurrent (m_window);

    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // start GLEW extension handler
    glewExperimental = GL_TRUE;
//    glewInit ();


    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
	/* Problem: glewInit failed, something is seriously wrong. */

	//	const string s = std::string(reinterpret_cast<const char*> (glewGetErrorString(err)));

	LOG_E("glewInit failed:%s", glewGetErrorString(err));

    }
    ClearOpenGLError();

    setupGLDebugMessages();

//    string se = "GL  VENDOR:" + tos(glGetString( GL_VENDOR) );

    LOG_I( "GL  VENDOR:%s", glGetString( GL_VENDOR) );
    LOG_I( "   VERSION:%s", glGetString( GL_VERSION ));
    LOG_I( "  RENDERER:%s", glGetString( GL_RENDERER ) );

    m_vao = new VAO();
    m_vao->Bind();
}

void Application::SetViewport() {


    int width, height;
    glfwGetFramebufferSize(m_window, &width, &height);

    ::SetViewport(0, 0, width, height);
}

int Application::GetFramebufferWidth() {
    int width, height;
    glfwGetFramebufferSize(m_window, &width, &height);

    return width;
}

int Application::GetFramebufferHeight() {
    int width, height;
    glfwGetFramebufferSize(m_window, &width, &height);

    return height;
}

void Application::Update_internal(const float delta) {

    const KeyboardState& kbs = KeyboardState::GetInstance();

    m_running = !kbs.IsPressed(GLFW_KEY_ESCAPE);// GetKey(GLFW_KEY_ESCAPE) != GLFW_PRESS ;

    Mouse::getInstance().Update(m_window);

    this->Update(delta);

}

int Application::GetWindowWidth() {
    int width, height;
    glfwGetWindowSize(m_window, &width, &height);

    return width;
}

int Application::GetWindowHeight() {
    int width, height;
    glfwGetWindowSize(m_window, &width, &height);
    return height;
}

void Application::SetWindowTitle(const std::string& title) {

    glfwSetWindowTitle(m_window, title.c_str());

}

void Application::RenderText_internal(const std::string& fpsString) {
    GL_C(glEnable (GL_BLEND));
    GL_C(glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));


    m_fontShader->Bind();

    RenderText();

    m_font->DrawString(*m_fontShader, 600,550, fpsString.substr(0,9) );
    m_fontShader->Unbind();

    GL_C(glDisable (GL_BLEND));
}
