#ifndef PARTICLE_SYSTEM_H
#define	PARTICLE_SYSTEM_H

#include "ewa/gl/gl_common.hpp"

#include "ewa/math/vector3f.hpp"

class Vector3f;
class Matrix4f;
class Matrix4f;
class Texture;
class ShaderProgram;
class VBO;
class RandomTexture;

class ParticleSystem
{
public:
    ParticleSystem();
    void Init();

    virtual ~ParticleSystem();

    virtual void Render(const Matrix4f& VP, const Vector3f& CameraPos);
    virtual void Update(float delta);

    void SetMinVelocity(const Vector3f& vel);
    void SetMaxVelocity(const Vector3f& vel);
    void SetVelocity(const Vector3f& vel);

    void SetMaxParticles(size_t maxParticles);
    void SetTexture(Texture* texture);
    void SetEmitRate(float emitRate);
    void SetParticleLifetime(float particleLifetime);
    void SetEmitPosition(const Vector3f& emitPosition);
    void SetEmitRange(const Vector3f& emitRange);
    void SetWarmupFrames(const int warmupFrames);
    void SetEmitCount(const int emitCount);

    void SetStartSize(const float startSize);
    void SetEndSize(const float endSize);
    void SetSize(const float size);


private:

    void UpdateParticles(float delta);
    void RenderParticles(const Matrix4f& VP, const Vector3f& CameraPos);


    bool m_isFirst;
    unsigned int m_currVB;
    unsigned int m_currTFB;
    VBO* m_particleBuffer[2];
    GLuint m_transformFeedback[2];

    ShaderProgram* m_particleUpdateShader;
    ShaderProgram* m_particleBillboardShader;
    RandomTexture* m_randomTexture;

    float m_time;


    Texture* m_texture;

    /*
      Minimum and maximum velocity
     */
    Vector3f m_minVelocity;
    Vector3f m_maxVelocity;

    size_t m_maxParticles;

    /*
      How often particles are emitted.
     */
    float m_emitRate;

    /*
      How many particles are emitted each time.
     */
    int m_emitCount;

    Vector3f m_emitPosition;
    Vector3f m_emitRange;
    float m_particleLifetime;

    /*
      The initial and final sizes.
     */
    float m_startSize;
    float m_endSize;

    int m_warmupFrames;


};


#endif	/* PARTICLE_SYSTEM_H */
