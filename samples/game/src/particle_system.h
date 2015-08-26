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
    void Init(const Vector3f& pos);

    ~ParticleSystem();

    void Render(const Matrix4f& VP, const Vector3f& CameraPos);
    void Update(float delta);

    void SetMinVelocity(const Vector3f& vel);
    void SetMaxVelocity(const Vector3f& vel);
    void SetMaxParticles(size_t maxParticles);
    void SetTexture(Texture* texture);
    void SetEmitRate(float emitRate);
    void SetParticleLifetime(float particleLifetime);
    void SetBillboardSize(float billboardSize);

private:

    bool m_isFirst;
    unsigned int m_currVB;
    unsigned int m_currTFB;
    VBO* m_particleBuffer[2];
    GLuint m_transformFeedback[2];

    ShaderProgram* m_particleUpdateShader;
    ShaderProgram* m_particleBillboardShader;
    RandomTexture* m_randomTexture;

    float m_time;

    Vector3f m_minVelocity;
    Vector3f m_maxVelocity;
    size_t m_maxParticles;
    Texture* m_texture;
    float m_emitRate;
    float m_particleLifetime;
    float m_billboardSize;
};


#endif	/* PARTICLE_SYSTEM_H */
