#include "ewa/log.hpp"

#include "ewa/gl/gl_util.hpp"


#include "ewa/random_texture.hpp"


using std::vector;

int main (int argc, char *argv[]) {

    LogInit();


    GLFWwindow* m_window;

    int m_width=800;int m_height=600;



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

    m_window = glfwCreateWindow (m_width, m_height, "Tuhu", NULL, NULL);
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



    LOG_I("hello world");


    size_t SIZE = 10;

    RandomTexture rt(SIZE,3);


    float* pixels = rt.GetPixels<float>(SIZE* 1 * 3 * sizeof(float), GL_RGB, GL_FLOAT);

    for(int i = 0; i < (SIZE*3 + 10); ++i) {
	LOG_I("pixels: %f", pixels[i] );
    }

    return 0;
}
