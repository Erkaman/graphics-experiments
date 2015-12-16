#pragma once

#include "particle_system.hpp"

class FireEffect : public ParticleSystem {

public:

    FireEffect(const Vector3f& pos);

    virtual ~FireEffect() {}
};
