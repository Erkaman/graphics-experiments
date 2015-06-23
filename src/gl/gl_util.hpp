#pragma once


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
