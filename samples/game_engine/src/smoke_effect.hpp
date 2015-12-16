#pragma once

#include "particle_system.hpp"

class SmokeEffect : public ParticleSystem {

public:

    SmokeEffect(const Vector3f& pos);

    virtual ~SmokeEffect() {}
};
