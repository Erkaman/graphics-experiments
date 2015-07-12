#pragma once

#include "gl_common.hpp"



void CheckOpenGLError(const char* stmt, const char* fname, int line);

void ClearOpenGLError();

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

void SetDepthTest(bool enable);

void SetViewport(GLint x, GLint y, GLsizei width, GLsizei height);

void Clear(GLclampf red, GLclampf green, GLclampf blue, GLbitfield mask = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
