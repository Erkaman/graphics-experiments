#include "application.h"


void Application::Start() {

    this->SetupOpenGL();
    this->Init();
    this->DoMainLoop();
    this->Cleanup();
}

void Application::DoMainLoop() {

}

void Application::Cleanup() {
    glfwTerminate();
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

    window = glfwCreateWindow (640, 480, "Tuhu", NULL, NULL);
    if (!window) {
	fprintf (stderr, "ERROR: could not open window with GLFW3\n");
	glfwTerminate();
	exit(1);
    }
    glfwMakeContextCurrent (window);

    // start GLEW extension handler
    glewExperimental = GL_TRUE;
    glewInit ();


    LOG_I( "GL  VENDOR: %s", glGetString( GL_VENDOR ) );
    LOG_I( "   VERSION: %s\n", glGetString( GL_VERSION ) );
    LOG_I( "  RENDERER: %s\n", glGetString( GL_RENDERER ) );
}
