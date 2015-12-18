#pragma once

#include "particle_system.hpp"

class SnowEffect : public ParticleSystem {

public:

    SnowEffect(const Vector3f& CameraPos);

    virtual ~SnowEffect() {}

    void Render(const Matrix4f& VP, const Vector3f& CameraPos);
};
