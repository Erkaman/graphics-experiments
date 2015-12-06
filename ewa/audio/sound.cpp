#include "sound.hpp"

#include "wave_loader.hpp"

//  "conditional expression is constant"
#pragma warning (disable : 4127)

Sound::Sound(WaveData* waveData) {
    AL_C(alGenBuffers(1, &m_buffer));

    AL_C(alBufferData(m_buffer, waveData->channels, waveData->audioData, waveData->audioDataSize, waveData->sampleRate));

    AL_C(alGenSources(1, &m_source));
    AL_C(alSourcei(m_source, AL_BUFFER, m_buffer));

}

Sound* Sound::Load(const std::string& filename) {


    WaveData* waveData = WaveLoader::Load(filename);

    if(!waveData)
	return NULL;

    return new Sound(waveData);
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

void Sound::SetLooping(bool looping) {
    AL_C(alSourcei(m_source, AL_LOOPING, looping ? AL_TRUE : AL_FALSE));
}
