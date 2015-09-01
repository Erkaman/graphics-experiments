#pragma once



#include <OpenAL/al.h>
#include <OpenAL/alc.h>

//#ifdef _DEBUG
    #define AL_C(stmt) do { \
            stmt; \
            CheckOpenALError(#stmt, __FILE__, __LINE__); \
        } while (0)
/*#else
    #define GL_C(stmt) stmt
    #endif*/

void CheckOpenALError(const char* stmt, const char* fname, int line);

class OpenAL {
public:
    static void Init();
};
