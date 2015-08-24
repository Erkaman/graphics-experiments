#include "ewa/log.hpp"

#include "ewa/gl/gl_util.hpp"


#include "ewa/gl/vbo.hpp"
#include "ewa/math/vector3f.hpp"
#include "ewa/common.hpp"


using std::vector;

// Shader macro
#define GLSL(src) "#version 150 core\n" #src

// Vertex shader
const GLchar* vertexShaderSrc = GLSL(
    in float inValue;
    out float geoValue;

    void main() {
        geoValue = sqrt(inValue);
    }
);

// Geometry shader
const GLchar* geoShaderSrc = GLSL(
    layout(points) in;
    layout(triangle_strip, max_vertices = 3) out;

    in float[] geoValue;
    out float outValue;

    void main() {
        for (int i = 0; i < 3; i++) {
            outValue = geoValue[0] + i;
            EmitVertex();
        }

        EndPrimitive();
    }
);



struct Particle
{
    float Type;
    Vector3f Pos;
    Vector3f Vel;
    float LifetimeMillis;
};

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





    // Compile shaders
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSrc, nullptr);
    glCompileShader(vertexShader);

    GLuint geoShader = glCreateShader(GL_GEOMETRY_SHADER);
    glShaderSource(geoShader, 1, &geoShaderSrc, nullptr);
    glCompileShader(geoShader);

    // Create program and specify transform feedback variables
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, geoShader);

    const GLchar* feedbackVaryings[] = { "outValue" };
    glTransformFeedbackVaryings(program, 1, feedbackVaryings, GL_INTERLEAVED_ATTRIBS);

    glLinkProgram(program);
    glUseProgram(program);

    // Create VAO
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Create input VBO and vertex format
    GLfloat data[] = { 1.0f, 2.0f, 3.0f, 4.0f, 5.0f };

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);

    GLint inputAttrib = glGetAttribLocation(program, "inValue");
    glEnableVertexAttribArray(inputAttrib);
    glVertexAttribPointer(inputAttrib, 1, GL_FLOAT, GL_FALSE, 0, 0);

    // Create transform feedback buffer
    GLuint tbo;
    glGenBuffers(1, &tbo);
    glBindBuffer(GL_ARRAY_BUFFER, tbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(data) * 3, nullptr, GL_STATIC_READ);

    // Create query object to collect info
    GLuint query;
    glGenQueries(1, &query);

    // Perform feedback transform
    glEnable(GL_RASTERIZER_DISCARD);



    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, tbo);



    glBeginQuery(GL_PRIMITIVES_GENERATED /*GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN*/, query);
        glBeginTransformFeedback(GL_TRIANGLES);
            glDrawArrays(GL_POINTS, 0, 5);
        glEndTransformFeedback();
	glEndQuery(GL_PRIMITIVES_GENERATED /*GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN*/);

    glDisable(GL_RASTERIZER_DISCARD);

    glFlush();

    // Fetch and print results
    GLuint primitives;
    glGetQueryObjectuiv(query, GL_QUERY_RESULT, &primitives);

    GLfloat feedback[15];
    glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, 0, sizeof(feedback), feedback);

    printf("%u primitives written!\n\n", primitives);

    for (int i = 0; i < 15; i++) {
        printf("%f\n", feedback[i]);
    }






    VBO* buffer = VBO::CreateInterleaved(
	vector<GLuint>{1,3,3,1}, // type, pos, vel, lifetime
	GL_DYNAMIC_DRAW

	);


    Particle Particles[1];

    Particles[0].Type = 1;
    Particles[0].Pos = Vector3f(0,0,0);
    Particles[0].Vel = Vector3f(0.0f, 0.0001f, 0.0f);
    Particles[0].LifetimeMillis = 0.0f;

    buffer->Bind();


    buffer->SetBufferData(sizeof(Particles), Particles);

    Particle arr;

    buffer->GetBufferSubData(0, sizeof(arr), &arr);
    LOG_I("type: %f", arr.Type);
    LOG_I("pos: %s", tos(arr.Pos).c_str() );
    LOG_I("vel: %s", tos(arr.Vel).c_str() );


    buffer->Unbind();




    buffer->Bind();



    buffer->Unbind();



    return 0;

//    printf("hello world\n");
}
