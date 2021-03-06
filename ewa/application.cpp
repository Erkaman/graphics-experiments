#include "application.hpp"

#include "gl/gl_common.hpp"
#include "gl/vao.hpp"
#include "gl/shader_program.hpp"
#include "gl/texture.hpp"
#include "gl/vbo.hpp"

#include "font.hpp"
#include "log.hpp"
#include "keyboard_state.hpp"
#include "resource_manager.hpp"

#include "math/vector3f.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#include <thread>


#include "fps_manager.hpp"
#include "mouse_state.hpp"
#include "config.hpp"

bool isFocused = true;

void WindowFocusCallback(GLFWwindow*, int iconified) {

    isFocused = iconified == GL_TRUE;

    LOG_I("is window: %d", isFocused);
}

using namespace std;

Application::Application(int argc, char *argv[], int width, int height): m_width(width), m_height(height) {
    for(int i = 1; i < argc; ++i) {
	ResourceManager::GetInstance().AddResourcePath(argv[i]);
    }

}

void Application::Start() {


    LogInit();

    this->SetupOpenGL();


    Config& config = Config::GetInstance();

/*    if(config.IsGui()) {
	m_guiVerticalScale = 0.3;

	int width, height;
	glfwGetFramebufferSize(m_window, &width, &height);

	m_framebufferWidth = width * (1.0 - m_guiVerticalScale);
	m_framebufferHeight = height;

    } else {
	m_guiVerticalScale = 0.0;

*/
    	m_guiVerticalScale = 0.3;
//	int width, height;
	glfwGetFramebufferSize(m_window, &m_framebufferWidth , &m_framebufferHeight);

	//  }


    m_fontShader = ShaderProgram::Load("shader_lib/font_render");

    if(!m_fontShader) {
	PrintErrorExit();
    }

    m_font = Font::Load(
	"img_lib/Ubuntu-B-64.png",
	"img_lib/Ubuntu-B-64.amf",

	GetFramebufferWidth(),GetFramebufferHeight(),
	0.8f);

    if(!m_font) {
	PrintErrorExit();
    }


    KeyboardState::GetInstance().Init(m_window);
    MouseState::GetInstance().Init(m_window);

    this->Init();
    this->DoMainLoop();
    this->Cleanup_internal();
}

void Application::DoMainLoop() {
    m_running = true;

    FPSManager fps;

    fps.Start();

    float delta = 0.0f;


    while(m_running) {


	if(!isFocused) {
	    // if lost focus, suspend the entire engine until focus is retained. This frees up the CPU, so that it can be used for other programs as well
	    glfwWaitEvents();
	}

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


void Application::Cleanup_internal() {

    Cleanup();

    m_vao->Unbind();
    delete m_vao;
    delete m_fontShader;
    delete m_font;

    glfwTerminate();
    LogDispose();

}


//    m_window = glfwCreateWindow (m_width, m_height, "Tuhu", NULL, NULL);

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error %d: %s\n", error, description);
}

void Application::SetupOpenGL() {

    // Setup window
    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
        LOG_E("Could not initialize GLFW");
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    m_window = glfwCreateWindow(m_width, m_height, "Tuhu", NULL, NULL);
    glfwMakeContextCurrent(m_window);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        LOG_E("Failed to initialize OpenGL context");
    }


    glfwSetWindowFocusCallback(m_window, WindowFocusCallback);

    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


    setupGLDebugMessages();

//    string se = "GL  VENDOR:" + tos(glGetString( GL_VENDOR) );

    LOG_I( "GL  VENDOR:%s", glGetString( GL_VENDOR) );
    LOG_I( "   VERSION:%s", glGetString( GL_VERSION ));
    LOG_I( "  RENDERER:%s", glGetString( GL_RENDERER ) );

    m_vao = new VAO();
    m_vao->Bind();
}

void Application::SetViewport() {

//    int width, height;
//    glfwGetFramebufferSize(m_window, &width, &height);

    ::SetViewport(0, 0, GetFramebufferWidth(), GetFramebufferHeight());
}

int Application::GetFramebufferWidth() {

    return m_framebufferWidth;

}

int Application::GetFramebufferHeight() {

    return m_framebufferHeight;
}

void Application::Update_internal(const float delta) {

    const KeyboardState& kbs = KeyboardState::GetInstance();

    m_running = !kbs.IsPressed(GLFW_KEY_ESCAPE);// GetKey(GLFW_KEY_ESCAPE) != GLFW_PRESS ;

    MouseState::GetInstance().Update();
    KeyboardState::GetInstance().Update();

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

    m_font->DrawString(*m_fontShader,

		       GetFramebufferWidth()-1100,
		       GetFramebufferHeight()-300,

		       fpsString.substr(0,9) );
    m_fontShader->Unbind();

    GL_C(glDisable (GL_BLEND));

}
