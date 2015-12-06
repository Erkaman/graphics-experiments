#pragma once

#include "openal.hpp"

#include <string>

class WaveData;

class Sound{

private:

    ALuint m_buffer;
    ALuint m_source;

    Sound(WaveData* waveData);


public:

    static Sound* Load(const std::string& filename);


    ~Sound();

    void Play();



    void SetPitch(float pitch);
    void SetGain(float gain);
    void SetLooping(bool looping);

};
