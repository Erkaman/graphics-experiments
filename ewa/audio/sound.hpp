#pragma once

#include "openal.hpp"

#include <string>

class Sound{

private:

    ALuint m_buffer;
    ALuint m_source;

public:

    Sound(const std::string& filename);
    ~Sound();

    void Play();



    void SetPitch(float pitch);
    void SetGain(float gain);
    void SetLooping(bool looping);

};
