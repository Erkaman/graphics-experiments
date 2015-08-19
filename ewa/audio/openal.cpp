#include "openal.hpp"

#include "ewa/log.hpp"

void OpenAL::Init() {
    alGetError();



    ALCdevice * deviceAL = alcOpenDevice(NULL);

    if (deviceAL == NULL) {
	LOG_E("Failed to init OpenAL device.");
	return;
    }

    ALCcontext * contextAL = alcCreateContext(deviceAL, NULL);

    AL_C( alcMakeContextCurrent(contextAL) );

    ALfloat orientation[] = { 0.f, 0.f, -1.f, 0.f, 1.f, 0.f };

    // initialize the listener.
    AL_C(alListener3f(AL_POSITION, 0,0,0));
    AL_C(alListener3f(AL_VELOCITY, 0,0,0));
    AL_C(alListenerfv(AL_ORIENTATION, orientation));


}

const char * GetOpenALErrorString(int errID)
{
	if (errID == AL_NO_ERROR) return "";
	if (errID == AL_INVALID_NAME) return "Invalid name";
    if (errID == AL_INVALID_ENUM) return " Invalid enum ";
    if (errID == AL_INVALID_VALUE) return " Invalid value ";
    if (errID == AL_INVALID_OPERATION) return " Invalid operation ";
    if (errID == AL_OUT_OF_MEMORY) return " Out of memory  ";

    return " Don't know ";
}

void CheckOpenALError(const char* stmt, const char* fname, int line)
{

    ALenum err = alGetError();

    const char* sError = GetOpenALErrorString(err);

    if (err != AL_NO_ERROR)
    {
        printf("OpenAL error %08x, at %s:%i - for %s. Error Message:%s\n", err, fname, line, stmt, sError);
	exit(1);
    }
}
