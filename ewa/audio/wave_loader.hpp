#pragma once

#include "wave_data.hpp"

#include <string>

class WaveLoader {

public:

    static WaveData Load(const std::string& filename);
};
