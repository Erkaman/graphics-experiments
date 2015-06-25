#include "application.hpp"

#include "common.hpp"

#include "gl/gl_common.hpp"


#include <stdio.h>
#include <stdlib.h>

using namespace std;

void Application::Start() {

    LogInit();

    this->SetupOpenGL();
    this->Init();
    this->DoMainLoop();
    this->Cleanup();
}

void Application::DoMainLoop() {
    running = true;
    while(running) {

	this->Update_internal();
	this->Render();

	// update frame buffer
	glfwSwapBuffers(window);
	// update input.
	glfwPollEvents();
    }
}

void Application::Cleanup() {
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

    window = glfwCreateWindow (640, 480, "Tuhu", NULL, NULL);
    if (!window) {
	fprintf (stderr, "ERROR: could not open window with GLFW3\n");
	glfwTerminate();
	exit(1);
    }
    glfwMakeContextCurrent (window);



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
}

void Application::SetViewport() {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
//    LOG_I("viewport: " + tos(width) + "," + tos(height) );
    GL_C(glViewport(0, 0, width, height));
}

void Application::Update_internal() {

    running = glfwGetKey( window , GLFW_KEY_ESCAPE ) != GLFW_PRESS ;

    this->Update();

}

int Application::GetWindowWidth() {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    return width;
}

int Application::GetWindowHeight() {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    return width;
}
