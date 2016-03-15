#pragma once

#include "math/vector3f.hpp"

class ParticleSystem;
class ViewFrustum;
class Gbuffer;
class ICamera;
class ShaderProgram;
class ColorFBO;

#include <vector>

class ParticlesRenderer {

private:
    ParticleSystem* m_fire;

    std::vector<Vector3f> m_particles;

    // used for compositing the particles.
    ShaderProgram* m_compositeShader;

    ColorFBO* m_particlesFbo;

    int m_particlesFboWidth;
    int m_particlesFboHeight;


public:
    ParticlesRenderer(int framebufferWidth, int framebufferHeight);

    void Update(ViewFrustum* cameraFrustum, const float delta);

    void Render(Gbuffer* gbuffer, ICamera* camera, int framebufferWidth, int framebufferHeight);



};
