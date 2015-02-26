#pragma once

#include "precomp.h"


inline void CheckOpenGLError(const char* stmt, const char* fname, int line)
{
    GLenum err = glGetError();
    const GLubyte* sError = gluErrorString(err);

    if (err != GL_NO_ERROR)
    {
        printf("OpenGL error %08x, at %s:%i - for %s. Error Message:%s\n", err, fname, line, stmt, sError);
	exit(1);
    }
}

inline void ClearOpenGLError() {
    while(glGetError() != GL_NO_ERROR) {}
}

// GL Check.
#ifdef _DEBUG
    #define GL_C(stmt) do { \
            stmt; \
            CheckOpenGLError(#stmt, __FILE__, __LINE__); \
        } while (0)
#else
    #define GL_C(stmt) stmt
#endif

void setupGLDebugMessages();
