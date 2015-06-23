#include "gl_util.hpp"

#include "common.hpp"
#include "gl_common.hpp"

using namespace std;

void handle_debug_message_( GLenum aSource, GLenum aType, GLuint aId, GLenum aSeverity, GLsizei /*aLength*/, const GLchar* aMessage, void* )
{



    // source string
    string srcStr = nullptr;
    switch (aSource) {
    case GL_DEBUG_SOURCE_API:				srcStr = "API";	break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:		srcStr = "WINDOW_SYSTEM"; break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER:	srcStr = "SHADER_COMPILER"; break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:		srcStr = "THIRD_PARTY"; break;
    case GL_DEBUG_SOURCE_APPLICATION:		srcStr = "APPLICATION"; break;
    case GL_DEBUG_SOURCE_OTHER:				srcStr = "OTHER"; break;
    default:								srcStr = "UNKNOWN";	break;
    }

    // type
    string  typeStr = nullptr;
    switch (aType)	{
    case GL_DEBUG_TYPE_ERROR:				typeStr = "ERROR";	break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:	typeStr = "DEPRECATED_BEHAVIOR";	break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:	typeStr = "UNDEFINED_BEHAVIOR";	break;
    case GL_DEBUG_TYPE_PORTABILITY:			typeStr = "PORTABILITY";	break;
    case GL_DEBUG_TYPE_PERFORMANCE:			typeStr = "PERFORMANCE";	break;
    case GL_DEBUG_TYPE_OTHER:				typeStr = "OTHER"; break;
    default:								typeStr = "UNKNOWN";	break;
    }

    // severity
    string sevStr = nullptr;
    switch( aSeverity ) {
    case GL_DEBUG_SEVERITY_HIGH:			sevStr = "HIGH"; break;
    case GL_DEBUG_SEVERITY_MEDIUM:			sevStr = "MEDIUM"; break;
    case GL_DEBUG_SEVERITY_LOW:				sevStr = "LOW";	break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:	sevStr = "NOTIFICATION"; break;
    default:								sevStr = "UNKNOWN";
    }

    // output message, if not just notification
    if (aSeverity != GL_DEBUG_SEVERITY_NOTIFICATION) {

	string s;


	s+= "\n";
	s+= "--\n";
	s+= "-- GL DEBUG MESSAGE:\n";
	s+= "--   severity = '" +sevStr + "'\n";
	s+= "--   type     = '" + typeStr + "'\n";
	s+=  "--   source   = '" + srcStr + "'\n";
	s+=  "--   id       = " + tos(aId) + "\n";
	s+= "-- message:\n";
	s+= tos(aMessage) + "\n";
	s+= "--\n";
	s+= "\n";

	fprintf(stderr, "%s", s.c_str());
	fflush(stderr);
    }

}


void setupGLDebugMessages()
{
    /* Make sure that we support this extension before attempting to do any-
     * thing with it...
     */
    if( !glewIsExtensionSupported( "GL_ARB_debug_output" ) )
    {
	LOG_W( "GL_ARB_debug_output not supported!" );
	return;
    }

    /* Enable debug messages. Set a callback handle, and then enable all
     * messages to begin with.
     */
    //glDebugMessageCallbackARB( &handle_debug_message_, 0 );
    GL_C(glDebugMessageCallbackARB((GLDEBUGPROC)&handle_debug_message_, nullptr));

    GL_C(glDebugMessageControlARB( GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, 0, true ));

    /* Enable synchronous debug output; this causes the callback to be called
     * immediately on error, usually in the actual gl-function where the error
     * occurred. (So, your stack trace is actually useful).
     *
     * This comes at a performance cost, so for performance sensitive stuff,
     * it might not be a good idea to unconditionally enable this. For the
     * labs, we just enable it, however.
     */
    glEnable( GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB );

    /* Debug output can be somewhat spammy, especially if all messages are
     * enabled. For now, disable the lowest level of messages, which mostly
     * contain performance-related information and other random notes.
     */
    glDebugMessageControlARB( GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_LOW_ARB, 0, 0, false );

    ClearOpenGLError();
}


void CheckOpenGLError(const char* stmt, const char* fname, int line)
{
    GLenum err = glGetError();
    const GLubyte* sError = gluErrorString(err);

    if (err != GL_NO_ERROR)
    {
        printf("OpenGL error %08x, at %s:%i - for %s. Error Message:%s\n", err, fname, line, stmt, sError);
	exit(1);
    }
}

void ClearOpenGLError() {
    while(glGetError() != GL_NO_ERROR) {}
}
