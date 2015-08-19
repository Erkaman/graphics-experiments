#include "sound.hpp"

#include "wave_loader.hpp"


Sound::Sound(const std::string& filename) {
    AL_C(alGenBuffers(1, &m_buffer));
    WaveData waveData = WaveLoader::Load(filename);
    AL_C(alBufferData(m_buffer, waveData.channels, waveData.audioData, waveData.audioDataSize, waveData.sampleRate));

    AL_C(alGenSources(1, &m_source));
    AL_C(alSourcei(m_source, AL_BUFFER, m_buffer));
}

Sound::~Sound() {
    AL_C(alDeleteSources(1, &m_source));
    AL_C(alDeleteBuffers(1, &m_buffer));
}

void Sound::Play() {
    alSourcePlay(m_source);
}

void Sound::SetPitch(float pitch){
    AL_C(alSourcef(m_source, AL_PITCH, pitch));
}

void Sound::SetGain(float gain){
    AL_C(alSourcef(m_source, AL_GAIN, gain));
}
