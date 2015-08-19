#pragma once

#include <stddef.h>

struct WaveData {
    void* audioData;
    size_t audioDataSize;

    int sampleRate;
    int channels;
};
