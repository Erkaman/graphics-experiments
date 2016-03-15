#pragma once

#include "math/vector3f.hpp"

class ParticleSystem;
class ViewFrustum;
class Gbuffer;
class ICamera;

#include <vector>

class ParticlesRenderer {

private:
    ParticleSystem* m_fire;

    std::vector<Vector3f> m_particles;

public:
    ParticlesRenderer();

    void Update(ViewFrustum* cameraFrustum, const float delta);

    void Render(Gbuffer* gbuffer, ICamera* camera, int framebufferWidth, int framebufferHeight);



};
